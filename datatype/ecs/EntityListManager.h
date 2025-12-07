#pragma once

#include "IEntityManager.h"
#include<hgl/type/SortedSet.h>

namespace hgl::ecs
{
    /**
     * CN: 列表式实体管理器
     * EN: List-based Entity Manager
     * 
     * CN: 使用有序集合管理实体，适合需要快速查找、添加、删除的场景。
     *     实体之间没有层级关系，只是简单的集合。
     * EN: Uses sorted set to manage entities, suitable for scenarios requiring
     *     fast lookup, addition, and deletion. Entities have no hierarchical
     *     relationships, just a simple collection.
     */
    class EntityListManager : public IEntityManager
    {
        SortedSet<EntityID> entity_set;

    public:

        EntityListManager(EntityPool* pool) : IEntityManager(pool) {}
        ~EntityListManager() override = default;

        /**
         * CN: 添加实体到列表
         * EN: Add entity to list
         */
        bool Add(const EntityID entity_id) override
        {
            // 检查实体是否存在于池中
            if (!entity_pool || !entity_pool->Contains(entity_id))
                return false;

            // 添加到集合（自动去重）
            return entity_set.Add(entity_id) >= 0;
        }

        /**
         * CN: 从列表移除实体
         * EN: Remove entity from list
         */
        bool Remove(const EntityID entity_id) override
        {
            return entity_set.Delete(entity_id);
        }

        /**
         * CN: 检查实体是否在列表中
         * EN: Check if entity is in list
         */
        bool Contains(const EntityID entity_id) const override
        {
            return entity_set.Contains(entity_id);
        }

        /**
         * CN: 获取列表中实体数量
         * EN: Get entity count in list
         */
        int GetCount() const override
        {
            return static_cast<int>(entity_set.GetCount());
        }

        /**
         * CN: 清空列表
         * EN: Clear list
         */
        void Clear() override
        {
            entity_set.Clear();
        }

        /**
         * CN: 遍历列表中的实体
         * EN: Iterate through entities in list
         */
        void Iterate(IterateFunc func, void* user_data = nullptr) override
        {
            if (!func)
                return;

            EntityID* data = entity_set.GetData();
            int64 count = entity_set.GetCount();

            for (int64 i = 0; i < count; ++i)
            {
                func(data[i], user_data);
            }
        }

        /**
         * CN: 获取底层的有序集合（用于高级操作）
         * EN: Get underlying sorted set (for advanced operations)
         */
        const SortedSet<EntityID>& GetSet() const
        {
            return entity_set;
        }

        /**
         * CN: 批量添加实体
         * EN: Batch add entities
         */
        int AddBatch(const EntityID* ids, int count)
        {
            if (!ids || count <= 0)
                return 0;

            int added = 0;
            for (int i = 0; i < count; ++i)
            {
                if (Add(ids[i]))
                    ++added;
            }
            return added;
        }

        /**
         * CN: 批量移除实体
         * EN: Batch remove entities
         */
        int RemoveBatch(const EntityID* ids, int count)
        {
            if (!ids || count <= 0)
                return 0;

            return static_cast<int>(entity_set.Delete(ids, count));
        }

        /**
         * CN: 获取指定索引的实体ID
         * EN: Get entity ID at specified index
         */
        EntityID GetAt(int index) const
        {
            EntityID id = ENTITY_ID_INVALID;
            entity_set.Get(index, id);
            return id;
        }
    };

} // namespace hgl::ecs
