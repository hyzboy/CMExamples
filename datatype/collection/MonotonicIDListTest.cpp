#include<hgl/type/DataArray.h>
#include<hgl/type/SortedSet.h>
#include<hgl/type/Stack.h>
#include<hgl/type/Map.h>

#include<iostream>

namespace hgl
{
    using MonotonicID=int32;

    struct IDUpdateItem
    {
        MonotonicID old_id;
        MonotonicID new_id;
        int old_location; // old location of the item
    };

    using IDUpdateList = DataArray<IDUpdateItem>;

    /**
    * 非模板：只管理 MonotonicID 与位置映射，隐藏实现细节
    */
    class DynamicMonoIDLocator
    {
        MonotonicID id_count;

        Map<MonotonicID,int> id_location_map;               ///<MonotonicID位置映射表
        SortedSet<MonotonicID> active_id_set;               ///<活跃MonotonicID集合
        Stack<int> free_location_set;                       ///<空闲位置集合

    public:

        DynamicMonoIDLocator():id_count(0){}

        MonotonicID AcquireID() { return ++id_count; }

        // 尝试弹出一个空闲位置，成功返回 true 并设置 location
        bool PopFreeLocation(int &location)
        {
            if(free_location_set.IsEmpty())
                return false;
            free_location_set.Pop(location);
            return true;
        }

        // 在指定位置注册新的 MonotonicID
        void RegisterIDAtLocation(MonotonicID id,int location)
        {
            id_location_map.Add(id,location);
            active_id_set.Add(id);
        }

        // 移除指定 MonotonicID
        bool Remove(MonotonicID id)
        {
            int location;

            if(!id_location_map.Get(id,location))
                return(false);

            active_id_set.Delete(id);
            id_location_map.DeleteByKey(id);
            free_location_set.Push(location);

            return(true);
        }

        // 根据 MonotonicID 获得位置
        bool GetLocation(MonotonicID id,int &location) const
        {
            return id_location_map.Get(id,location);
        }

        //访问底层映射数据（用于遍历）
        int GetMapCount() const { return id_location_map.GetCount(); }
        auto GetDataList() const { return id_location_map.GetDataList(); } // KeyValue<MonotonicID,int>**

        int BuildCompactMapping(IDUpdateList &mapping) const
        {
            int src_count = GetMapCount();

            mapping.Resize(src_count);

            if(src_count<=0)
                return 0;

            IDUpdateItem *ip=mapping.GetData();

            auto id_loc=id_location_map.GetDataList();

            MonotonicID new_id=0;

            for(int i=0;i<src_count;i++)
            {
                ip->old_id = (*id_loc)->key;
                ip->new_id = new_id;
                ip->old_location = (*id_loc)->value;

                ++ip;
                ++new_id;
                ++id_loc;
            }

            return new_id;
        }

        void Reset(int new_id_count)
        {
            id_count = (new_id_count>0)?(new_id_count-1):-1;
            free_location_set.Clear();

            id_location_map.Clear();
            active_id_set.Resize(new_id_count);

            for(int i=0;i<new_id_count;i++)
            {
                id_location_map.Add(i,i);
                active_id_set.Add(i);
            }
        }
    };

    /**
    * 动态单调MonotonicID列表<br>
    *只针对原生数据类型，不支持对象数据类型
    */
    template<typename T>
    class DynamicMonoIDList
    {
        DataArray<T> item_list; // 存放数据
        DynamicMonoIDLocator locator; // 管理 MonotonicID 与位置

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
            AddLocation(loc); // ignore returned MonotonicID
            return loc;
        }

        bool Remove(MonotonicID id)
        {
            return locator.Remove(id);
        }

        MonotonicID Add(const T &data)
        {
            int loc;
            MonotonicID new_id = AddLocation(loc);
            if(loc<0) return static_cast<MonotonicID>(-1);

            memcpy(item_list.At(loc), &data, sizeof(T));
            return new_id;
        }

        // 重载：AddLocation 返回新的 MonotonicID 与位置（避免 AcquireID 重复递增）
        MonotonicID AddLocation(int &out_location)
        {
            MonotonicID new_id = locator.AcquireID();
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

        // 根据 MonotonicID 获取元素指针，找不到返回 nullptr
        T *GetByID(MonotonicID id)
        {
            int location;
            if(!locator.GetLocation(id, location))
                return nullptr;

            if(location <0)
                return nullptr;

            return reinterpret_cast<T *>(item_list.At(location));
        }

        // 将数据重新紧密排列，并从0 开始重新编号。
        // 若 mapping 不为 nullptr，则在其中写入旧 MonotonicID -> 新 MonotonicID 的映射（以 IDUpdateItem 存放）。
        bool Compact(IDUpdateList *mapping)
        {
            if(GetCount() <= 0)
                return(true);

            if(!mapping)
                return(false)

            locator.BuildCompactMapping(mapping);
            {

            }
            locator.Reset(item_list.GetCount());
        }
    };//class DynamicMonoIDList
}//namespace hgl

using namespace hgl;
using namespace std;

int main(int,char **)
{
}
