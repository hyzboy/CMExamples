#include<hgl/type/DataArray.h>
#include<hgl/type/SortedSet.h>
#include<hgl/type/Stack.h>
#include<hgl/type/Map.h>

#include<iostream>

namespace hgl
{
    struct IDUpdateItem
    {
        int old_id;
        int new_id;
    };

    using IDUpdateList = DataArray<IDUpdateItem>;

    /**
    * 非模板：只管理 ID 与位置映射，隐藏实现细节
    */
    class DynamicMonoIDLocator
    {
    public:
        using ID = int32;

    private:
        ID id_count;

        Map<ID,int> id_location_map;                    ///<ID位置映射表
        SortedSet<ID> active_id_set;                      ///<活跃ID集合
        Stack<int> free_location_set;                  ///<空闲位置集合

    public:
        DynamicMonoIDLocator():id_count(0){}

        ID AcquireID() { return ++id_count; }

        // 尝试弹出一个空闲位置，成功返回 true 并设置 location
        bool PopFreeLocation(int &location)
        {
            if(free_location_set.IsEmpty())
                return false;
            free_location_set.Pop(location);
            return true;
        }

        // 在指定位置注册新的 ID
        void RegisterIDAtLocation(ID id,int location)
        {
            id_location_map.Add(id,location);
            active_id_set.Add(id);
        }

        // 移除指定 ID
        bool Remove(ID id)
        {
            int location;
            if(!id_location_map.Get(id,location))
                return(false);

            active_id_set.Delete(id);
            id_location_map.DeleteByKey(id);
            free_location_set.Push(location);

            return(true);
        }

        // 根据 ID 获得位置
        bool GetLocation(ID id,int &location) const
        {
            return id_location_map.Get(id,location);
        }

        //访问底层映射数据（用于遍历）
        int GetMapCount() const { return id_location_map.GetCount(); }
        auto GetDataList() const { return id_location_map.GetDataList(); } // KeyValue<ID,int>**

        // 应用 Compact 的结果（接管新的映射/集合）
        void ApplyCompactResult(Map<ID,int> &&new_map, SortedSet<ID> &&new_active_set, Stack<int> &&new_free_set, ID new_id_count)
        {
            id_location_map = std::move(new_map);
            active_id_set = std::move(new_active_set);
            free_location_set = std::move(new_free_set);
            id_count = new_id_count;
        }
    };

    /**
    * 动态单调ID列表<br>
    *只针对原生数据类型，不支持对象数据类型
    */
    template<typename T>
    class DynamicMonoIDList
    {
        using ID=int32;

        DataArray<T> item_list; // 存放数据
        DynamicMonoIDLocator locator; // 管理 ID 与位置

    protected:

        int GetCount() const
        {
            return item_list.GetCount();
        }

        void Expand(int size)
        {
            item_list.Expand(size);
        }

        void *GetItemAt(int location)
        {
            return item_list.At(location);
        }

    public:

        DynamicMonoIDList(){}
        virtual ~DynamicMonoIDList() = default;

        // 原始返回位置的接口，保持兼容
        int AddLocation()
        {
            int loc;
            AddLocation(loc); // ignore returned ID
            return loc;
        }

        bool Remove(ID id)
        {
            return locator.Remove(id);
        }

        ID Add(const T &data)
        {
            int loc;
            ID new_id = AddLocation(loc);
            if(loc<0) return static_cast<ID>(-1);

            memcpy(item_list.At(loc), &data, sizeof(T));
            return new_id;
        }

        // 重载：AddLocation 返回新的 ID 与位置（避免 AcquireID 重复递增）
        ID AddLocation(int &out_location)
        {
            ID new_id = locator.AcquireID();
            int location;
            if(!locator.PopFreeLocation(location))
            {
                location = GetCount();
                Expand(1);
            }
            locator.RegisterIDAtLocation(new_id, location);
            out_location = location;
            return new_id;
        }

        // 根据 ID 获取元素指针，找不到返回 nullptr
        T *GetByID(ID id)
        {
            int location;
            if(!locator.GetLocation(id, location))
                return nullptr;

            if(location <0)
                return nullptr;

            return reinterpret_cast<T *>(item_list.At(location));
        }

        // 将数据重新紧密排列，并从0 开始重新编号。
        // 若 mapping 不为 nullptr，则在其中写入旧 ID -> 新 ID 的映射（以 IDUpdateItem 存放）。
        void Compact(IDUpdateList *mapping=nullptr)
        {
            DataArray<T> new_items;
            Map<ID,int> new_id_location_map;
            SortedSet<ID> new_active_set;
            Stack<int> new_free_set; // 将保持为空

            int new_index =0;

            // 遍历 locator 的 id_location_map 条目
            int src_count = locator.GetMapCount();
            auto src_list = locator.GetDataList(); // KeyValue<ID,int>**

            //先计算有效条目数，以便为 mapping 一次性分配
            int valid_count =0;
            for(int i=0;i<src_count;i++)
            {
                auto kv = src_list[i];
                if(!kv) continue;
                if(kv->value <0) continue;
                ++valid_count;
            }

            IDUpdateItem *map_data = nullptr;
            int map_index =0;
            if(mapping)
            {
                mapping->Resize(valid_count);
                map_data = mapping->GetData();
            }

            for(int i=0;i<src_count;i++)
            {
                auto kv = src_list[i];
                if(!kv) continue;

                ID old_id = kv->key;
                int location = kv->value;

                if(location <0) continue;

                int dest = new_items.GetCount();
                new_items.Expand(1);
                memcpy(new_items.At(dest), item_list.At(location), sizeof(T));

                ID new_id = static_cast<ID>(new_index);

                new_id_location_map.Add(new_id, dest);
                new_active_set.Add(new_id);

                if(map_data)
                {
                    map_data[map_index].old_id = old_id;
                    map_data[map_index].new_id = new_id;
                    ++map_index;
                }

                ++new_index;
            }

            // 将新的映射应用到 locator
            ID new_id_count = (new_index>0)? static_cast<ID>(new_index-1) : static_cast<ID>(-1);
            locator.ApplyCompactResult(std::move(new_id_location_map), std::move(new_active_set), std::move(new_free_set), new_id_count);

            // 替换数据区
            item_list = std::move(new_items);
        }

    };//class DynamicMonoIDList
}

using namespace hgl;
using namespace std;

int main(int,char **)
{
}
