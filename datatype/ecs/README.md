# ECS 系统设计文档 / ECS System Design Document

## 概述 / Overview

这是一个灵活的实体组件系统(ECS)设计，核心特点是**将Entity与管理结构完全分离**。实体池独立管理所有实体，而管理结构只持有实体ID的引用。

This is a flexible Entity Component System (ECS) design with the core feature of **complete separation between Entities and Management Structures**. The entity pool independently manages all entities, while management structures only hold references to entity IDs.

## 核心设计原则 / Core Design Principles

### 1. 分离关注点 (Separation of Concerns)
- **EntityPool**: 负责实体的生命周期管理（创建、销毁、查询）
- **IEntityManager**: 定义管理结构的通用接口
- **具体管理器**: 实现不同的组织方式（列表、树、图等）
- **ComponentPool**: 管理组件数据

### 2. 灵活性 (Flexibility)
- 同一个实体可以同时存在于多个管理结构中
- 可以根据需求选择不同的管理结构
- 组件系统与管理结构解耦

### 3. 性能 (Performance)
- 使用MonotonicIDList确保ID永不重复
- 组件数据紧密排列，缓存友好
- 支持批量操作

## 架构组件 / Architecture Components

### EntityPool (实体池)

**文件**: `ecs/EntityPool.h`

**职责**:
- 集中管理所有实体的创建和销毁
- 提供实体的查询接口
- 管理实体的附加数据（flags, user_data）

**主要接口**:
```cpp
EntityID Create()                           // 创建新实体
bool Destroy(EntityID id)                   // 销毁实体
Entity* Get(EntityID id)                    // 获取实体指针
bool Contains(EntityID id)                  // 检查实体是否存在
void* GetUserData(EntityID id)              // 获取用户数据
bool SetUserData(EntityID id, void* data)   // 设置用户数据
```

### IEntityManager (管理器接口)

**文件**: `ecs/IEntityManager.h`

**职责**:
- 定义管理结构的统一接口
- 所有管理器必须实现此接口

**主要接口**:
```cpp
virtual bool Add(EntityID entity_id) = 0;          // 添加实体
virtual bool Remove(EntityID entity_id) = 0;       // 移除实体
virtual bool Contains(EntityID entity_id) = 0;     // 检查包含
virtual int GetCount() const = 0;                  // 获取数量
virtual void Clear() = 0;                          // 清空
virtual void Iterate(IterateFunc func) = 0;        // 遍历
```

### EntityListManager (列表管理器)

**文件**: `ecs/EntityListManager.h`

**特点**:
- 使用有序集合(SortedSet)管理实体
- 适合平面结构，无层级关系
- 支持快速查找、添加、删除

**使用场景**:
- 同类型实体集合（如：所有敌人、所有子弹）
- 需要快速遍历的实体组
- 活跃实体列表

**主要接口**:
```cpp
bool Add(EntityID entity_id)                      // 添加实体
bool Remove(EntityID entity_id)                   // 移除实体
int AddBatch(const EntityID* ids, int count)      // 批量添加
int RemoveBatch(const EntityID* ids, int count)   // 批量移除
EntityID GetAt(int index)                         // 按索引获取
```

### EntityTreeManager (树形管理器)

**文件**: `ecs/EntityTreeManager.h`

**特点**:
- 管理父子层级关系
- 支持多根节点
- 深度优先遍历
- 支持节点提升（删除节点但保留子节点）

**使用场景**:
- 场景图（Scene Graph）
- UI层级结构
- 游戏对象父子关系
- 组织结构树

**主要接口**:
```cpp
bool AddAsRoot(EntityID entity_id)                    // 添加为根节点
bool AddAsChild(EntityID entity_id, EntityID parent)  // 添加为子节点
bool Remove(EntityID entity_id)                       // 移除节点及子节点
bool RemoveOnly(EntityID entity_id)                   // 仅移除节点
EntityID GetParent(EntityID entity_id)                // 获取父节点
int GetChildCount(EntityID entity_id)                 // 获取子节点数
void IterateDFS(EntityID id, IterateFunc func)        // 深度优先遍历
void IterateChildren(EntityID id, IterateFunc func)   // 遍历子节点
```

### ComponentPool (组件池)

**文件**: `ecs/ComponentPool.h`

**特点**:
- 模板类，支持任意组件类型
- 组件与实体一对一关联
- 支持快速查询和遍历

**使用方法**:
```cpp
// 定义组件
struct PositionComponent {
    float x, y, z;
};

// 创建组件池
ComponentPool<PositionComponent> positions;

// 添加组件
PositionComponent* pos = positions.Add(entity_id);
pos->x = 10.0f;

// 查询组件
if (positions.Has(entity_id)) {
    PositionComponent* pos = positions.Get(entity_id);
}

// 遍历组件
positions.Iterate([](EntityID id, PositionComponent* comp, void* data) {
    // 处理每个组件
});
```

## 使用示例 / Usage Examples

### 示例1: 简单实体管理

```cpp
#include "ecs/EntityPool.h"
#include "ecs/EntityListManager.h"

EntityPool pool;
EntityListManager enemies(&pool);

// 创建敌人
for (int i = 0; i < 10; ++i) {
    EntityID enemy = pool.Create();
    enemies.Add(enemy);
}

// 遍历所有敌人
enemies.Iterate([](EntityID id, void* data) {
    // 更新敌人逻辑
});
```

### 示例2: 场景树管理

```cpp
#include "ecs/EntityPool.h"
#include "ecs/EntityTreeManager.h"

EntityPool pool;
EntityTreeManager scene(&pool);

// 创建场景树
EntityID root = pool.Create();
scene.AddAsRoot(root);

EntityID child1 = pool.Create();
scene.AddAsChild(child1, root);

EntityID child2 = pool.Create();
scene.AddAsChild(child2, root);

EntityID grandchild = pool.Create();
scene.AddAsChild(grandchild, child1);

// 深度优先遍历
scene.Iterate([](EntityID id, void* data) {
    // 处理每个节点
});
```

### 示例3: 完整ECS系统

```cpp
#include "ecs/EntityPool.h"
#include "ecs/EntityTreeManager.h"
#include "ecs/ComponentPool.h"

// 定义组件
struct TransformComponent {
    float x, y, z;
    float rotation;
};

struct RenderComponent {
    int texture_id;
    int mesh_id;
};

// 创建系统
EntityPool pool;
EntityTreeManager scene(&pool);
ComponentPool<TransformComponent> transforms;
ComponentPool<RenderComponent> renders;

// 创建游戏对象
EntityID player = pool.Create();
scene.AddAsRoot(player);
transforms.Add(player, TransformComponent{0, 0, 0, 0});
renders.Add(player, RenderComponent{1, 1});

// 创建武器（作为玩家的子对象）
EntityID weapon = pool.Create();
scene.AddAsChild(weapon, player);
transforms.Add(weapon, TransformComponent{1, 0, 0, 0});
renders.Add(weapon, RenderComponent{2, 2});

// 渲染系统：遍历所有有渲染组件的实体
renders.Iterate([&](EntityID id, RenderComponent* render, void*) {
    TransformComponent* transform = transforms.Get(id);
    if (transform) {
        // 渲染逻辑
        DrawMesh(render->mesh_id, render->texture_id, 
                 transform->x, transform->y, transform->z);
    }
});
```

## 高级特性 / Advanced Features

### 多管理器同时使用

一个实体可以同时存在于多个管理结构中：

```cpp
EntityPool pool;
EntityListManager all_entities(&pool);      // 所有实体
EntityListManager active_enemies(&pool);    // 活跃的敌人
EntityTreeManager scene_graph(&pool);       // 场景层级

EntityID enemy = pool.Create();
all_entities.Add(enemy);
active_enemies.Add(enemy);
scene_graph.AddAsChild(enemy, world_root);

// 从活跃列表移除，但保留在场景树中
active_enemies.Remove(enemy);
// enemy 仍在 scene_graph 和 all_entities 中
```

### 实体查询系统

可以实现基于组件的查询：

```cpp
// 查询所有同时拥有Transform和Render组件的实体
void QueryRenderableEntities(
    EntityListManager& entities,
    ComponentPool<TransformComponent>& transforms,
    ComponentPool<RenderComponent>& renders,
    std::function<void(EntityID, TransformComponent*, RenderComponent*)> callback)
{
    entities.Iterate([&](EntityID id, void*) {
        if (transforms.Has(id) && renders.Has(id)) {
            callback(id, transforms.Get(id), renders.Get(id));
        }
    });
}
```

### 系统设计模式

```cpp
// 系统基类
class System {
protected:
    EntityPool* entity_pool;
public:
    System(EntityPool* pool) : entity_pool(pool) {}
    virtual void Update(float delta_time) = 0;
};

// 物理系统
class PhysicsSystem : public System {
    ComponentPool<TransformComponent>& transforms;
    ComponentPool<PhysicsComponent>& physics;
    
public:
    void Update(float dt) override {
        physics.Iterate([&](EntityID id, PhysicsComponent* phy, void*) {
            TransformComponent* trans = transforms.Get(id);
            if (trans) {
                // 更新物理和位置
                phy->velocity += phy->acceleration * dt;
                trans->x += phy->velocity.x * dt;
                trans->y += phy->velocity.y * dt;
            }
        });
    }
};
```

## 性能考虑 / Performance Considerations

### 内存管理

1. **定期收缩**: 删除大量实体后调用 `Shrink()` 回收内存
```cpp
pool.Shrink();
component_pool.Shrink();
```

2. **预分配**: 如果知道实体数量，提前分配
```cpp
// 预分配空间以避免频繁重新分配
```

### 缓存友好

- 组件数据紧密排列在内存中
- 遍历组件比遍历实体更高效
- 按组件类型批处理更新

### 最佳实践

1. **按系统组织**: 每个系统只关心特定组件
2. **批量操作**: 使用批量添加/删除接口
3. **避免频繁查询**: 缓存常用的组件指针
4. **合理使用管理结构**: 
   - 列表 - 简单集合、频繁遍历
   - 树 - 层级关系、空间划分

## 扩展建议 / Extension Suggestions

### 1. 图形管理器

可以实现 `EntityGraphManager` 支持任意关系：

```cpp
class EntityGraphManager : public IEntityManager {
    // 实体可以有多个"父"和多个"子"
    Map<EntityID, SortedSet<EntityID>> edges;
};
```

### 2. 空间管理器

```cpp
class EntitySpatialManager : public IEntityManager {
    // 基于四叉树或八叉树的空间划分
    QuadTree<EntityID> spatial_index;
};
```

### 3. 事件系统

```cpp
class EntityEventSystem {
    // 实体事件订阅和分发
    Map<EventType, SortedSet<EntityID>> listeners;
};
```

### 4. 组件依赖

```cpp
// 自动添加依赖组件
template<typename T, typename... Deps>
class ComponentPoolWithDeps : public ComponentPool<T> {
    // 添加T时自动添加Deps...
};
```

## 文件结构 / File Structure

```
Examples/datatype/
├── EcsTest.cpp                      # 测试程序
└── ecs/
    ├── EntityPool.h                 # 实体池
    ├── IEntityManager.h             # 管理器接口
    ├── EntityListManager.h          # 列表管理器
    ├── EntityTreeManager.h          # 树管理器
    └── ComponentPool.h              # 组件池
```

## 总结 / Summary

这个ECS设计的优势：

1. **灵活性**: Entity与管理结构分离，支持多种组织方式
2. **可扩展**: 易于添加新的管理器类型和组件
3. **高性能**: 组件紧密排列，缓存友好
4. **清晰性**: 职责分离，代码易于理解和维护
5. **复用性**: 同一实体可存在于多个管理结构

适用场景：
- 游戏引擎
- 模拟系统
- 复杂的对象管理
- 场景图系统
- 需要灵活组织结构的应用

---

**作者**: ECS System Design
**日期**: 2024
**版本**: 1.0
