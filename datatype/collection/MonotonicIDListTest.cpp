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
    * 动态单调ID列表<br>
    * 只针对原生数据类型，不支持对象数据类型
    */
    template<typename T> 
    class DynamicMonoIDList
    {
        using ID=int32;

        ID id_count;

        DataArray<T>    item_list;
        
        Map<ID,int>     id_location_map;                    ///<ID位置映射表
        SortedSet<ID>   active_id_set;                      ///<活跃ID集合
        Stack<int>      free_location_set;                  ///<空闲位置集合

        ID AcquireID(){return ++id_count;}

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

        DynamicMonoIDList():id_count(0){}
        virtual ~DynamicMonoIDList() = default;

        int AddLocation()
        {
            ID new_id=AcquireID();

            active_id_set.Add(new_id);

            if(free_location_set.IsEmpty())
            {
                int location=GetCount();
                Expand(1);
                id_location_map.Add(new_id,location);
                return location;
            }
            else
            {
                int location;
                free_location_set.Pop(location);
                id_location_map.Add(new_id,location);
                return location;
            }
        }

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

        ID Add(const T &data)
        {
            int loc=AddLocation();

            if(loc<0)
                return(-1);

            memcpy(item_list.At(loc),&data,sizeof(T));
            return(id_count);
        }

        // 根据 ID 获取元素指针，找不到返回 nullptr
        T *GetByID(ID id)
        {
            int location;
            if(!id_location_map.Get(id, location))
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

            // 遍历现有 id_location_map 的条目，避免从1..id_count的无谓循环
            int src_count = id_location_map.GetCount();
            auto src_list = id_location_map.GetDataList(); // KeyValue<ID,int>**

            //先计算有效条目数量，以便一次性为 mapping预分配空间
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

                // 在 new_items 中扩展一个槽并复制数据（仅针对原生 POD 类型安全）
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

            // 替换旧的数据结构
            item_list = std::move(new_items);
            id_location_map = std::move(new_id_location_map);
            active_id_set = std::move(new_active_set);
            free_location_set = std::move(new_free_set);

            // 更新 id_count
            if(new_index >0)
                id_count = static_cast<ID>(new_index -1);
            else
                id_count = -1;
        }

    };//class DynamicMonoIDList
}//namespace hgl

using namespace hgl;
using namespace std;

int main(int,char **)
{
}
