#pragma once

#include "EntityPool.h"
#include<hgl/type/Map.h>

namespace hgl::ecs
{
    /**
     * CN: 组件池模板类
     * EN: Component Pool Template Class
     * 
     * CN: 为特定类型的组件提供池化管理，支持组件与实体的关联。
     *     每个组件池管理一种类型的组件。
     * EN: Provides pooled management for specific component types,
     *     supports component-entity association.
     *     Each component pool manages one type of component.
     * 
     * @tparam T CN: 组件类型 / EN: Component type
     */
    template<typename T>
    class ComponentPool
    {
        struct ComponentData
        {
            EntityID entity_id;     ///< CN: 关联的实体ID / EN: Associated entity ID
            T component;            ///< CN: 组件数据 / EN: Component data

            ComponentData() : entity_id(ENTITY_ID_INVALID) {}
        };

        MonotonicIDList<ComponentData, MonotonicID> component_list;
        Map<EntityID, MonotonicID> entity_to_component;  ///< CN: 实体ID到组件ID的映射 / EN: Entity ID to component ID mapping

    public:

        ComponentPool() = default;
        ~ComponentPool() = default;

        /**
         * CN: 为实体添加组件
         * EN: Add component to entity
         * @param entity_id CN: 实体ID / EN: Entity ID
         * @return CN: 返回组件指针，失败返回nullptr / EN: Returns component pointer, or nullptr on failure
         */
        T* Add(const EntityID entity_id)
        {
            // 检查是否已经有组件
            if (entity_to_component.ContainsKey(entity_id))
                return nullptr;

            ComponentData* data = component_list.Add();
            if (!data)
                return nullptr;

            data->entity_id = entity_id;
            
            // 获取组件ID（使用当前位置作为ID）
            MonotonicID comp_id = static_cast<MonotonicID>(component_list.GetLocation(data->entity_id));
            
            entity_to_component.Add(entity_id, comp_id);

            return &data->component;
        }

        /**
         * CN: 添加组件并设置初始值
         * EN: Add component with initial value
         */
        bool Add(const EntityID entity_id, const T& component)
        {
            T* comp = Add(entity_id);
            if (!comp)
                return false;

            *comp = component;
            return true;
        }

        /**
         * CN: 移除实体的组件
         * EN: Remove entity's component
         */
        bool Remove(const EntityID entity_id)
        {
            MonotonicID* comp_id_ptr = entity_to_component.Get(entity_id);
            if (!comp_id_ptr)
                return false;

            MonotonicID comp_id = *comp_id_ptr;
            entity_to_component.DeleteByKey(entity_id);
            
            return component_list.Remove(comp_id);
        }

        /**
         * CN: 获取实体的组件
         * EN: Get entity's component
         */
        T* Get(const EntityID entity_id)
        {
            MonotonicID* comp_id_ptr = entity_to_component.Get(entity_id);
            if (!comp_id_ptr)
                return nullptr;

            ComponentData* data = component_list.Get(*comp_id_ptr);
            return data ? &data->component : nullptr;
        }

        /**
         * CN: 获取实体的组件（常量）
         * EN: Get entity's component (const)
         */
        const T* Get(const EntityID entity_id) const
        {
            return const_cast<ComponentPool<T>*>(this)->Get(entity_id);
        }

        /**
         * CN: 检查实体是否有此类型组件
         * EN: Check if entity has this component type
         */
        bool Has(const EntityID entity_id) const
        {
            return entity_to_component.ContainsKey(entity_id);
        }

        /**
         * CN: 获取组件总数
         * EN: Get component count
         */
        int GetCount() const
        {
            return entity_to_component.GetCount();
        }

        /**
         * CN: 清空所有组件
         * EN: Clear all components
         */
        void Clear()
        {
            component_list.Shrink();
            entity_to_component.Clear();
        }

        /**
         * CN: 收缩内存
         * EN: Shrink memory
         */
        int Shrink()
        {
            return component_list.Shrink();
        }

        /**
         * CN: 遍历所有组件
         * EN: Iterate through all components
         */
        using IterateFunc = void(*)(EntityID entity_id, T* component, void* user_data);
        
        void Iterate(IterateFunc func, void* user_data = nullptr)
        {
            if (!func)
                return;

            auto** keys = entity_to_component.GetKeyList();
            auto** values = entity_to_component.GetDataList();
            int count = entity_to_component.GetCount();

            for (int i = 0; i < count; ++i)
            {
                ComponentData* data = component_list.Get(values[i]);
                if (data)
                {
                    func(keys[i], &data->component, user_data);
                }
            }
        }
    };

    /**
     * CN: 组件管理器 - 管理多种类型的组件池
     * EN: Component Manager - Manages multiple component pool types
     * 
     * CN: 这是一个示例基类，实际使用时可以继承它并添加具体的组件池
     * EN: This is an example base class, inherit it and add specific component pools in practice
     */
    class ComponentManager
    {
    protected:
        EntityPool* entity_pool;

    public:

        ComponentManager(EntityPool* pool) : entity_pool(pool) {}
        virtual ~ComponentManager() = default;

        EntityPool* GetEntityPool() const { return entity_pool; }

        /**
         * CN: 当实体被销毁时，移除其所有组件
         * EN: Remove all components when entity is destroyed
         */
        virtual void OnEntityDestroyed(EntityID entity_id) = 0;

        /**
         * CN: 清空所有组件
         * EN: Clear all components
         */
        virtual void Clear() = 0;
    };

} // namespace hgl::ecs
