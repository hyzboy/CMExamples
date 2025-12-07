#pragma once

#include "IEntityManager.h"
#include<hgl/type/Map.h>
#include<hgl/type/SortedSet.h>

namespace hgl::ecs
{
    /**
     * CN: 树节点数据结构
     * EN: Tree node data structure
     */
    struct TreeNode
    {
        EntityID entity_id;         ///< CN: 当前节点的实体ID / EN: Entity ID of current node
        EntityID parent_id;         ///< CN: 父节点ID / EN: Parent node ID
        SortedSet<EntityID> children; ///< CN: 子节点ID集合 / EN: Child node ID set

        TreeNode() : entity_id(ENTITY_ID_INVALID), parent_id(ENTITY_ID_INVALID) {}

        void Init(EntityID id, EntityID parent = ENTITY_ID_INVALID)
        {
            entity_id = id;
            parent_id = parent;
            children.Clear();
        }

        bool HasChildren() const { return children.GetCount() > 0; }
        bool IsRoot() const { return parent_id == ENTITY_ID_INVALID; }
    };

    /**
     * CN: 树形实体管理器
     * EN: Tree-based Entity Manager
     * 
     * CN: 管理具有父子层级关系的实体，每个实体可以有一个父节点和多个子节点。
     *     适合场景图、UI层级、组织结构等树形场景。
     * EN: Manages entities with parent-child hierarchical relationships.
     *     Each entity can have one parent node and multiple child nodes.
     *     Suitable for scene graphs, UI hierarchies, organizational structures, etc.
     */
    class EntityTreeManager : public IEntityManager
    {
        Map<EntityID, TreeNode*> node_map;  ///< CN: 实体ID到节点的映射 / EN: Entity ID to node mapping
        SortedSet<EntityID> root_nodes;     ///< CN: 根节点集合 / EN: Root node set

    private:

        /**
         * CN: 递归删除节点及其所有子节点
         * EN: Recursively delete node and all its children
         */
        void RemoveNodeRecursive(TreeNode* node)
        {
            if (!node)
                return;

            // 先递归删除所有子节点
            EntityID* children = node->children.GetData();
            int64 count = node->children.GetCount();

            for (int64 i = 0; i < count; ++i)
            {
                TreeNode** child_node_ptr = node_map.Get(children[i]);
                if (child_node_ptr)
                {
                    RemoveNodeRecursive(*child_node_ptr);
                }
            }

            // 从父节点的子节点列表中移除
            if (!node->IsRoot())
            {
                TreeNode** parent_node_ptr = node_map.Get(node->parent_id);
                if (parent_node_ptr)
                {
                    (*parent_node_ptr)->children.Delete(node->entity_id);
                }
            }
            else
            {
                root_nodes.Delete(node->entity_id);
            }

            // 从映射表中移除
            node_map.DeleteByKey(node->entity_id);
            delete node;
        }

    public:

        EntityTreeManager(EntityPool* pool) : IEntityManager(pool) {}

        ~EntityTreeManager() override
        {
            Clear();
        }

        /**
         * CN: 添加实体作为根节点
         * EN: Add entity as root node
         */
        bool Add(const EntityID entity_id) override
        {
            return AddAsRoot(entity_id);
        }

        /**
         * CN: 添加实体作为根节点
         * EN: Add entity as root node
         */
        bool AddAsRoot(const EntityID entity_id)
        {
            // 检查实体是否存在于池中
            if (!entity_pool || !entity_pool->Contains(entity_id))
                return false;

            // 检查是否已经在树中
            if (node_map.ContainsKey(entity_id))
                return false;

            TreeNode* node = new TreeNode();
            node->Init(entity_id);

            node_map.Add(entity_id, node);
            root_nodes.Add(entity_id);

            return true;
        }

        /**
         * CN: 添加实体作为指定父节点的子节点
         * EN: Add entity as child of specified parent node
         */
        bool AddAsChild(const EntityID entity_id, const EntityID parent_id)
        {
            // 检查实体是否存在于池中
            if (!entity_pool || !entity_pool->Contains(entity_id))
                return false;

            // 检查父节点是否存在
            TreeNode** parent_node_ptr = node_map.Get(parent_id);
            if (!parent_node_ptr)
                return false;

            // 检查是否已经在树中
            if (node_map.ContainsKey(entity_id))
                return false;

            TreeNode* node = new TreeNode();
            node->Init(entity_id, parent_id);

            node_map.Add(entity_id, node);
            (*parent_node_ptr)->children.Add(entity_id);

            return true;
        }

        /**
         * CN: 移除实体及其所有子节点
         * EN: Remove entity and all its children
         */
        bool Remove(const EntityID entity_id) override
        {
            TreeNode** node_ptr = node_map.Get(entity_id);
            if (!node_ptr)
                return false;

            RemoveNodeRecursive(*node_ptr);
            return true;
        }

        /**
         * CN: 仅移除实体，将其子节点提升到父节点
         * EN: Remove entity only, promote its children to parent
         */
        bool RemoveOnly(const EntityID entity_id)
        {
            TreeNode** node_ptr = node_map.Get(entity_id);
            if (!node_ptr)
                return false;

            TreeNode* node = *node_ptr;

            // 将所有子节点移到父节点或根节点
            EntityID* children = node->children.GetData();
            int64 count = node->children.GetCount();

            if (node->IsRoot())
            {
                // 如果是根节点，子节点全部变为根节点
                for (int64 i = 0; i < count; ++i)
                {
                    TreeNode** child_ptr = node_map.Get(children[i]);
                    if (child_ptr)
                    {
                        (*child_ptr)->parent_id = ENTITY_ID_INVALID;
                        root_nodes.Add(children[i]);
                    }
                }

                root_nodes.Delete(entity_id);
            }
            else
            {
                // 子节点挂到父节点下
                TreeNode** parent_ptr = node_map.Get(node->parent_id);
                if (parent_ptr)
                {
                    for (int64 i = 0; i < count; ++i)
                    {
                        TreeNode** child_ptr = node_map.Get(children[i]);
                        if (child_ptr)
                        {
                            (*child_ptr)->parent_id = node->parent_id;
                            (*parent_ptr)->children.Add(children[i]);
                        }
                    }

                    (*parent_ptr)->children.Delete(entity_id);
                }
            }

            node_map.DeleteByKey(entity_id);
            delete node;

            return true;
        }

        /**
         * CN: 检查实体是否在树中
         * EN: Check if entity is in tree
         */
        bool Contains(const EntityID entity_id) const override
        {
            return node_map.ContainsKey(entity_id);
        }

        /**
         * CN: 获取树中节点总数
         * EN: Get total node count in tree
         */
        int GetCount() const override
        {
            return node_map.GetCount();
        }

        /**
         * CN: 获取根节点数量
         * EN: Get root node count
         */
        int GetRootCount() const
        {
            return static_cast<int>(root_nodes.GetCount());
        }

        /**
         * CN: 获取节点的子节点数量
         * EN: Get child count of a node
         */
        int GetChildCount(const EntityID entity_id) const
        {
            TreeNode** node_ptr = const_cast<Map<EntityID, TreeNode*>&>(node_map).Get(entity_id);
            if (!node_ptr)
                return 0;

            return static_cast<int>((*node_ptr)->children.GetCount());
        }

        /**
         * CN: 获取节点的父节点ID
         * EN: Get parent node ID
         */
        EntityID GetParent(const EntityID entity_id) const
        {
            TreeNode** node_ptr = const_cast<Map<EntityID, TreeNode*>&>(node_map).Get(entity_id);
            if (!node_ptr)
                return ENTITY_ID_INVALID;

            return (*node_ptr)->parent_id;
        }

        /**
         * CN: 检查节点是否为根节点
         * EN: Check if node is root
         */
        bool IsRoot(const EntityID entity_id) const
        {
            TreeNode** node_ptr = const_cast<Map<EntityID, TreeNode*>&>(node_map).Get(entity_id);
            if (!node_ptr)
                return false;

            return (*node_ptr)->IsRoot();
        }

        /**
         * CN: 清空整个树
         * EN: Clear entire tree
         */
        void Clear() override
        {
            // 删除所有节点
            auto** nodes = node_map.GetDataList();
            int count = node_map.GetCount();

            for (int i = 0; i < count; ++i)
            {
                delete nodes[i];
            }

            node_map.Clear();
            root_nodes.Clear();
        }

        /**
         * CN: 深度优先遍历
         * EN: Depth-first traversal
         */
        void Iterate(IterateFunc func, void* user_data = nullptr) override
        {
            if (!func)
                return;

            // 遍历所有根节点
            EntityID* roots = root_nodes.GetData();
            int64 root_count = root_nodes.GetCount();

            for (int64 i = 0; i < root_count; ++i)
            {
                IterateDFS(roots[i], func, user_data);
            }
        }

        /**
         * CN: 递归深度优先遍历
         * EN: Recursive depth-first traversal
         */
        void IterateDFS(EntityID entity_id, IterateFunc func, void* user_data)
        {
            if (!func)
                return;

            TreeNode** node_ptr = node_map.Get(entity_id);
            if (!node_ptr)
                return;

            // 先访问当前节点
            func(entity_id, user_data);

            // 递归访问所有子节点
            TreeNode* node = *node_ptr;
            EntityID* children = node->children.GetData();
            int64 count = node->children.GetCount();

            for (int64 i = 0; i < count; ++i)
            {
                IterateDFS(children[i], func, user_data);
            }
        }

        /**
         * CN: 遍历某个节点的所有子节点
         * EN: Iterate through all children of a node
         */
        void IterateChildren(EntityID entity_id, IterateFunc func, void* user_data = nullptr)
        {
            if (!func)
                return;

            TreeNode** node_ptr = node_map.Get(entity_id);
            if (!node_ptr)
                return;

            TreeNode* node = *node_ptr;
            EntityID* children = node->children.GetData();
            int64 count = node->children.GetCount();

            for (int64 i = 0; i < count; ++i)
            {
                func(children[i], user_data);
            }
        }

        /**
         * CN: 获取所有根节点
         * EN: Get all root nodes
         */
        const SortedSet<EntityID>& GetRoots() const
        {
            return root_nodes;
        }
    };

} // namespace hgl::ecs
