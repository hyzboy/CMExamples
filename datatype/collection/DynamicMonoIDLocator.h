#pragma once

#include<hgl/type/DataArray.h>
#include<hgl/type/SortedSet.h>
#include<hgl/type/Stack.h>
#include<hgl/type/Map.h>

#include<vector>

namespace hgl
{
    using MonotonicID = int32;

    // Final ID mapping (old_id -> new_id)
    struct IDUpdateItem
    {
        MonotonicID old_id;
        MonotonicID new_id;
    };

    using IDUpdateList = DataArray<IDUpdateItem>;

    // Location mapping used for the first step (compacting locations)
    struct LocationUpdateItem
    {
        MonotonicID old_id;
        int old_location; // original location
        int new_location; // new compacted location
    };

    using LocationUpdateList = DataArray<LocationUpdateItem>;

    /**
    * 非模板：只管理 MonotonicID 与位置映射，隐藏实现细节
    */
    class DynamicMonoIDLocator
    {
        MonotonicID id_count;

        Map<MonotonicID,int> id_location_map; ///<MonotonicID位置映射表
        SortedSet<MonotonicID> active_id_set; ///<活跃MonotonicID集合
        Stack<int> free_location_set; ///<空闲位置集合

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

        // Build final compact mapping (old_id -> new_id) based on current map order
        int BuildCompactMapping(IDUpdateList &mapping) const;

        // 将 id_location_map 紧密化，但不改变 ID；可选 mapping 输出 old_id->new_location 并包含旧位置
        // 返回新的项数量
        int CompactLocations(LocationUpdateList *mapping = nullptr);

        void Reset(int new_id_count);
    };
}
