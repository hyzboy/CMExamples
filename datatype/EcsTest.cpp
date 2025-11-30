#include "ecs/EntityPool.h"
#include "ecs/EntityListManager.h"
#include "ecs/EntityTreeManager.h"
#include "ecs/ComponentPool.h"
#include <iostream>

using namespace hgl::ecs;

// 示例组件：位置组件
struct PositionComponent
{
    float x, y, z;

    PositionComponent() : x(0), y(0), z(0) {}
    PositionComponent(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
};

// 示例组件：名称组件
struct NameComponent
{
    char name[64];

    NameComponent() { name[0] = 0; }
    
    void SetName(const char* n)
    {
        if (!n) return;
        strncpy_s(name, n, 63);
        name[63] = 0;
    }
};

// 打印实体ID的回调函数
void PrintEntity(EntityID entity_id, void* user_data)
{
    std::cout << "  Entity ID: " << entity_id << std::endl;
}

// 打印位置组件的回调函数
void PrintPosition(EntityID entity_id, PositionComponent* component, void* user_data)
{
    std::cout << "  Entity " << entity_id 
              << " Position: (" << component->x 
              << ", " << component->y 
              << ", " << component->z << ")" << std::endl;
}

void TestEntityPool()
{
    std::cout << "\n=== Test EntityPool ===" << std::endl;
    
    EntityPool pool;

    // 创建实体
    EntityID e1 = pool.Create();
    EntityID e2 = pool.Create();
    EntityID e3 = pool.Create();

    std::cout << "Created entities: " << e1 << ", " << e2 << ", " << e3 << std::endl;

    // 检查实体存在
    std::cout << "Entity " << e2 << " exists: " << (pool.Contains(e2) ? "Yes" : "No") << std::endl;

    // 销毁实体
    pool.Destroy(e2);
    std::cout << "After destroying " << e2 << ", exists: " << (pool.Contains(e2) ? "Yes" : "No") << std::endl;

    // 再创建一个实体
    EntityID e4 = pool.Create();
    std::cout << "Created new entity: " << e4 << std::endl;
}

void TestEntityListManager()
{
    std::cout << "\n=== Test EntityListManager ===" << std::endl;

    EntityPool pool;
    EntityListManager list_mgr(&pool);

    // 创建实体并添加到列表
    EntityID e1 = pool.Create();
    EntityID e2 = pool.Create();
    EntityID e3 = pool.Create();

    list_mgr.Add(e1);
    list_mgr.Add(e2);
    list_mgr.Add(e3);

    std::cout << "List contains " << list_mgr.GetCount() << " entities:" << std::endl;
    list_mgr.Iterate(PrintEntity);

    // 移除一个实体
    list_mgr.Remove(e2);
    std::cout << "\nAfter removing entity " << e2 << ":" << std::endl;
    list_mgr.Iterate(PrintEntity);
}

void TestEntityTreeManager()
{
    std::cout << "\n=== Test EntityTreeManager ===" << std::endl;

    EntityPool pool;
    EntityTreeManager tree_mgr(&pool);

    // 创建实体
    EntityID root = pool.Create();
    EntityID child1 = pool.Create();
    EntityID child2 = pool.Create();
    EntityID grandchild = pool.Create();

    // 构建树结构
    tree_mgr.AddAsRoot(root);
    tree_mgr.AddAsChild(child1, root);
    tree_mgr.AddAsChild(child2, root);
    tree_mgr.AddAsChild(grandchild, child1);

    std::cout << "Tree structure:" << std::endl;
    std::cout << "Root: " << root << std::endl;
    std::cout << "  Child1: " << child1 << std::endl;
    std::cout << "    Grandchild: " << grandchild << std::endl;
    std::cout << "  Child2: " << child2 << std::endl;

    std::cout << "\nDepth-first traversal:" << std::endl;
    tree_mgr.Iterate(PrintEntity);

    std::cout << "\nRoot " << root << " has " << tree_mgr.GetChildCount(root) << " children" << std::endl;
    std::cout << "Child1 " << child1 << " parent is: " << tree_mgr.GetParent(child1) << std::endl;
}

void TestComponentPool()
{
    std::cout << "\n=== Test ComponentPool ===" << std::endl;

    EntityPool pool;
    ComponentPool<PositionComponent> pos_pool;
    ComponentPool<NameComponent> name_pool;

    // 创建实体
    EntityID e1 = pool.Create();
    EntityID e2 = pool.Create();
    EntityID e3 = pool.Create();

    // 添加位置组件
    PositionComponent* pos1 = pos_pool.Add(e1);
    pos1->x = 10.0f; pos1->y = 20.0f; pos1->z = 30.0f;

    pos_pool.Add(e2, PositionComponent(100.0f, 200.0f, 300.0f));

    // 添加名称组件
    NameComponent* name1 = name_pool.Add(e1);
    name1->SetName("Player");

    NameComponent* name2 = name_pool.Add(e2);
    name2->SetName("Enemy");

    // 查询组件
    std::cout << "Entity " << e1 << " has position: " 
              << (pos_pool.Has(e1) ? "Yes" : "No") << std::endl;
    std::cout << "Entity " << e3 << " has position: " 
              << (pos_pool.Has(e3) ? "Yes" : "No") << std::endl;

    // 遍历所有位置组件
    std::cout << "\nAll position components:" << std::endl;
    pos_pool.Iterate(PrintPosition);

    // 获取组件
    NameComponent* name = name_pool.Get(e1);
    if (name)
    {
        std::cout << "\nEntity " << e1 << " name: " << name->name << std::endl;
    }
}

void TestIntegratedSystem()
{
    std::cout << "\n=== Test Integrated System ===" << std::endl;

    // 创建池和管理器
    EntityPool pool;
    EntityTreeManager scene_graph(&pool);
    ComponentPool<PositionComponent> positions;
    ComponentPool<NameComponent> names;

    // 创建场景树：Root -> [Object1, Object2] -> [SubObject1]
    EntityID root = pool.Create();
    scene_graph.AddAsRoot(root);
    names.Add(root)->SetName("Scene Root");
    positions.Add(root, PositionComponent(0, 0, 0));

    EntityID obj1 = pool.Create();
    scene_graph.AddAsChild(obj1, root);
    names.Add(obj1)->SetName("Object1");
    positions.Add(obj1, PositionComponent(10, 0, 0));

    EntityID obj2 = pool.Create();
    scene_graph.AddAsChild(obj2, root);
    names.Add(obj2)->SetName("Object2");
    positions.Add(obj2, PositionComponent(-10, 0, 0));

    EntityID subobj1 = pool.Create();
    scene_graph.AddAsChild(subobj1, obj1);
    names.Add(subobj1)->SetName("SubObject1");
    positions.Add(subobj1, PositionComponent(5, 5, 0));

    std::cout << "Scene graph with " << scene_graph.GetCount() << " entities:" << std::endl;

    // 遍历场景树并打印信息
    auto print_hierarchy = [&](EntityID entity_id, void* user_data)
    {
        int depth = *static_cast<int*>(user_data);
        
        for (int i = 0; i < depth; ++i)
            std::cout << "  ";

        NameComponent* name = names.Get(entity_id);
        PositionComponent* pos = positions.Get(entity_id);

        std::cout << "- Entity " << entity_id;
        if (name) std::cout << " (" << name->name << ")";
        if (pos) std::cout << " at [" << pos->x << ", " << pos->y << ", " << pos->z << "]";
        std::cout << std::endl;
    };

    // 手动递归打印（因为lambda无法直接递归）
    std::function<void(EntityID, int)> print_tree;
    print_tree = [&](EntityID entity_id, int depth)
    {
        for (int i = 0; i < depth; ++i)
            std::cout << "  ";

        NameComponent* name = names.Get(entity_id);
        PositionComponent* pos = positions.Get(entity_id);

        std::cout << "- Entity " << entity_id;
        if (name) std::cout << " (" << name->name << ")";
        if (pos) std::cout << " at [" << pos->x << ", " << pos->y << ", " << pos->z << "]";
        std::cout << std::endl;

        // 打印子节点
        scene_graph.IterateChildren(entity_id, [](EntityID child_id, void* data)
        {
            auto& func = *static_cast<std::function<void(EntityID, int)>*>(data);
            func(child_id, *static_cast<int*>(data) + 1);
        }, nullptr);
    };

    // 从根节点开始打印
    int depth = 0;
    const SortedSet<EntityID>& roots = scene_graph.GetRoots();
    EntityID* root_data = roots.GetData();
    for (int64 i = 0; i < roots.GetCount(); ++i)
    {
        print_tree(root_data[i], depth);
    }
}

int main()
{
    std::cout << "==================================" << std::endl;
    std::cout << "    ECS System Test Suite" << std::endl;
    std::cout << "==================================" << std::endl;

    TestEntityPool();
    TestEntityListManager();
    TestEntityTreeManager();
    TestComponentPool();
    TestIntegratedSystem();

    std::cout << "\n==================================" << std::endl;
    std::cout << "    All Tests Completed!" << std::endl;
    std::cout << "==================================" << std::endl;

    return 0;
}