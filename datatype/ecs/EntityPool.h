#pragma once

#include<hgl/type/MonotonicIDList.h>

namespace hgl::ecs
{
    using EntityID = MonotonicID;
    constexpr const EntityID ENTITY_ID_INVALID = 0;

    /**
     * CN: 实体基础数据结构
     * EN: Entity base data structure
     */
    struct Entity
    {
        EntityID entity_id;         ///< CN: 实体唯一ID / EN: Unique entity ID
        uint32   flags;             ///< CN: 标志位，可用于状态管理 / EN: Flags for state management
        void*    user_data;         ///< CN: 用户自定义数据指针 / EN: User-defined data pointer

        void Init()
        {
            entity_id = ENTITY_ID_INVALID;
            flags = 0;
            user_data = nullptr;
        }

        bool IsValid() const { return entity_id > ENTITY_ID_INVALID; }
    };

    /**
     * CN: 实体池 - 集中管理所有实体
     * EN: Entity Pool - Centralized entity management
     * 
     * CN: 实体池负责实体的创建、销毁和查询，与管理结构完全解耦。
     *     管理结构只需要持有EntityID引用即可。
     * EN: Entity pool handles entity creation, destruction and queries,
     *     completely decoupled from management structures.
     *     Management structures only need to hold EntityID references.
     */
    class EntityPool
    {
        MonotonicIDList<Entity, EntityID> entity_list;

    public:

        EntityPool() = default;
        ~EntityPool() = default;

        /**
         * CN: 创建一个新实体
         * EN: Create a new entity
         * @return CN: 返回新实体的ID，失败返回ENTITY_ID_INVALID
         *         EN: Returns the new entity ID, or ENTITY_ID_INVALID on failure
         */
        EntityID Create()
        {
            Entity* entity = entity_list.Add();
            if (!entity)
                return ENTITY_ID_INVALID;

            entity->Init();
            entity->entity_id = static_cast<EntityID>(entity_list.GetLocation(entity->entity_id) + 1);
            
            return entity->entity_id;
        }

        /**
         * CN: 销毁一个实体
         * EN: Destroy an entity
         * @param id CN: 要销毁的实体ID / EN: Entity ID to destroy
         * @return CN: 成功返回true / EN: Returns true on success
         */
        bool Destroy(const EntityID id)
        {
            if (!entity_list.Contains(id))
                return false;

            return entity_list.Remove(id);
        }

        /**
         * CN: 获取实体指针
         * EN: Get entity pointer
         * @param id CN: 实体ID / EN: Entity ID
         * @return CN: 返回实体指针，不存在返回nullptr / EN: Returns entity pointer, or nullptr if not found
         */
        Entity* Get(const EntityID id)
        {
            return entity_list.Get(id);
        }

        /**
         * CN: 获取实体指针（常量）
         * EN: Get entity pointer (const)
         */
        const Entity* Get(const EntityID id) const
        {
            return const_cast<EntityPool*>(this)->entity_list.Get(id);
        }

        /**
         * CN: 检查实体是否存在
         * EN: Check if entity exists
         */
        bool Contains(const EntityID id) const
        {
            return entity_list.Contains(id);
        }

        /**
         * CN: 获取实体的用户数据
         * EN: Get entity user data
         */
        template<typename T>
        T* GetUserData(const EntityID id)
        {
            Entity* entity = Get(id);
            return entity ? static_cast<T*>(entity->user_data) : nullptr;
        }

        /**
         * CN: 设置实体的用户数据
         * EN: Set entity user data
         */
        bool SetUserData(const EntityID id, void* data)
        {
            Entity* entity = Get(id);
            if (!entity)
                return false;

            entity->user_data = data;
            return true;
        }

        /**
         * CN: 获取/设置实体标志位
         * EN: Get/Set entity flags
         */
        uint32 GetFlags(const EntityID id) const
        {
            const Entity* entity = Get(id);
            return entity ? entity->flags : 0;
        }

        bool SetFlags(const EntityID id, uint32 flags)
        {
            Entity* entity = Get(id);
            if (!entity)
                return false;

            entity->flags = flags;
            return true;
        }

        /**
         * CN: 收缩内存，移除碎片
         * EN: Shrink memory, remove fragmentation
         */
        int Shrink()
        {
            return entity_list.Shrink();
        }

        /**
         * CN: 重新编号所有实体ID
         * EN: Reindex all entity IDs
         */
        int Reindex()
        {
            return entity_list.Reindex();
        }
    };

} // namespace hgl::ecs
