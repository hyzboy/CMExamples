#include "DynamicMonoIDLocator.h"

#include<hgl/type/DataArray.h>
#include<hgl/type/SortedSet.h>
#include<hgl/type/Stack.h>
#include<hgl/type/Map.h>

#include<iostream>
#include<vector>

namespace hgl
{
    using MonotonicID = int32;

    // DynamicMonoIDLocator and mapping types are defined in the separate header

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
            if(loc < 0) return static_cast<MonotonicID>(-1);

            memcpy(item_list.At(loc),&data,sizeof(T));
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
            locator.RegisterIDAtLocation(new_id,location);
            out_location = location;
            return new_id;
        }

        // 根据 MonotonicID 获取元素指针，找不到返回 nullptr
        T *GetByID(MonotonicID id)
        {
            int location;
            if(!locator.GetLocation(id,location))
                return nullptr;

            if(location < 0)
                return nullptr;

            return reinterpret_cast<T *>(item_list.At(location));
        }

        // 将数据重新紧密排列，并从0 开始重新编号。
        // 若 id_mapping 不为 nullptr，则在其中写入旧 MonotonicID -> NewMonotonicID 的映射（以 IDUpdateItem 存放）。
        bool Compact(IDUpdateList *id_mapping)
        {
            if(GetCount() <= 0)
                return(true);

            // 第一步：紧缩 locator 内的位置并获得 location mapping
            LocationUpdateList loc_map;
            int new_count = locator.CompactLocations(&loc_map);
            if(new_count <= 0)
            {
                // nothing left
                item_list.Clear();
                locator.Reset(0);
                if(id_mapping) id_mapping->Resize(0);
                return true;
            }

            // 第二步：根据 loc_map.old_location 把数据搬移到新的紧凑数组
            LocationUpdateItem *loc_data = loc_map.GetData();

            DataArray<T> new_items;
            new_items.Resize(new_count);

            for(int i = 0;i < new_count;i++)
            {
                int old_loc = loc_data[i].old_location;
                // copy POD data
                hgl_cpy<T>(new_items.At(i),item_list.At(old_loc),1);
            }

            // 替换数据区
            item_list = std::move(new_items);

            // 第三步：重新生成 ID 与位置为0..new_count-1
            locator.Reset(new_count);

            // 如果需要 final id mapping（old_id -> new_id），写入 id_mapping
            if(id_mapping)
            {
                id_mapping->Resize(new_count);
                IDUpdateItem *mid = id_mapping->GetData();
                for(int i = 0;i < new_count;i++)
                {
                    mid[i].old_id = loc_data[i].old_id;
                    mid[i].new_id = static_cast<MonotonicID>(i);
                }
            }

            return true;
        }

    };//class DynamicMonoIDList
}//namespace hgl

using namespace hgl;
using namespace std;

int main(int,char **)
{
}
