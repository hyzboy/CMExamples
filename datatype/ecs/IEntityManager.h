#pragma once

#include "EntityPool.h"

namespace hgl::ecs
{
    /**
     * CN: 实体管理器接口
     * EN: Entity Manager Interface
     * 
     * CN: 定义了管理结构的通用接口，不同的管理结构可以继承此接口实现
     *     各自的管理策略（列表、树、图等）
     * EN: Defines common interface for management structures. Different management
     *     structures can inherit this interface to implement their own strategies
     *     (list, tree, graph, etc.)
     */
    class IEntityManager
    {
    protected:
        EntityPool* entity_pool;    ///< CN: 关联的实体池 / EN: Associated entity pool

    public:

        IEntityManager(EntityPool* pool) : entity_pool(pool) {}
        virtual ~IEntityManager() = default;

        /**
         * CN: 获取关联的实体池
         * EN: Get associated entity pool
         */
        EntityPool* GetEntityPool() const { return entity_pool; }

        /**
         * CN: 添加实体到管理结构
         * EN: Add entity to management structure
         * @param entity_id CN: 要添加的实体ID / EN: Entity ID to add
         * @return CN: 成功返回true / EN: Returns true on success
         */
        virtual bool Add(const EntityID entity_id) = 0;

        /**
         * CN: 从管理结构移除实体（不销毁实体）
         * EN: Remove entity from management structure (doesn't destroy entity)
         * @param entity_id CN: 要移除的实体ID / EN: Entity ID to remove
         * @return CN: 成功返回true / EN: Returns true on success
         */
        virtual bool Remove(const EntityID entity_id) = 0;

        /**
         * CN: 检查实体是否在此管理结构中
         * EN: Check if entity is in this management structure
         */
        virtual bool Contains(const EntityID entity_id) const = 0;

        /**
         * CN: 获取管理的实体数量
         * EN: Get number of managed entities
         */
        virtual int GetCount() const = 0;

        /**
         * CN: 清空管理结构（不销毁实体）
         * EN: Clear management structure (doesn't destroy entities)
         */
        virtual void Clear() = 0;

        /**
         * CN: 遍历所有管理的实体
         * EN: Iterate through all managed entities
         * @param func CN: 回调函数 / EN: Callback function
         */
        using IterateFunc = void(*)(EntityID entity_id, void* user_data);
        virtual void Iterate(IterateFunc func, void* user_data = nullptr) = 0;
    };

} // namespace hgl::ecs
