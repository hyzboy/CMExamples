# 大小核（Big.LITTLE）CPU检测功能

## 概述

HGL库现在支持检测ARM处理器的大小核架构，这对于移动设备和嵌入式系统特别重要。大核和小核技术允许处理器在高性能需求时使用大核，在低功耗场景时使用小核，从而平衡性能和电池续航。

## 架构支持

### 支持的CPU架构
- **x86_64**: 传统桌面/服务器处理器
- **ARMv8**: ARM 64位处理器，支持大小核架构
- **ARMv9**: 最新的ARM 64位处理器

## 大小核检测

### 检测原理

大小核检测通过以下方式实现：

1. **系统信息分析**: 通过 `GetNativeSystemInfo()` 获取处理器信息
2. **簇分析**: 分析处理器簇的数量和配置
3. **频率差异**: 检测不同核簇之间的频率差异
4. **缓存配置**: 分析缓存层级和大小差异

### 检测逻辑

```cpp
// 简化的检测逻辑示例
bool has_big_little = (processor_count > 4); // 多核通常有大小核

if (has_big_little) {
    big_core_count = processor_count / 2;
    little_core_count = processor_count - big_core_count;
}
```

## 使用方法

### 基本使用

```cpp
#include <hgl/platform/CpuInfo.h>

CpuInfo ci;

if (GetCpuInfo(&ci)) {
    if (ci.arch == CpuArch::ARMv8 || ci.arch == CpuArch::ARMv9) {
        const CpuARMFeatures& arm = ci.arm;

        if (arm.has_big_little) {
            std::cout << "Big cores: " << arm.big_core_count << std::endl;
            std::cout << "Little cores: " << arm.little_core_count << std::endl;
            std::cout << "Big core freq: " << arm.big_core_max_freq << " MHz" << std::endl;
            std::cout << "Little core freq: " << arm.little_core_max_freq << " MHz" << std::endl;
        }
    }
}
```

### 高级使用

```cpp
// 任务调度优化
void OptimizeTaskScheduling(const CpuInfo& ci) {
    if (ci.arch == CpuArch::ARMv8 && ci.arm.has_big_little) {
        // 为计算密集型任务分配大核
        ScheduleHeavyTasksOnBigCores(ci.arm.big_core_count);

        // 为轻量级任务分配小核
        ScheduleLightTasksOnLittleCores(ci.arm.little_core_count);
    }
}

// 功耗管理
void ManagePowerConsumption(const CpuInfo& ci) {
    if (ci.arm.has_big_little) {
        // 在电池模式下优先使用小核
        if (IsBatteryMode()) {
            PreferLittleCores();
        } else {
            UseAllCores();
        }
    }
}
```

## 数据结构

### CpuARMFeatures 结构

```cpp
struct CpuARMFeatures {
    // 基本信息
    char implementer[3];           // 实现商
    uint32 part_number;            // 部件号
    uint32 variant;                // 变体
    uint32 revision;               // 修订版

    // 指令集支持
    bool has_neon;                 // NEON SIMD
    bool has_vfpv4;                // VFPv4浮点
    bool has_aes;                  // AES加密
    bool has_sha1, has_sha2;       // SHA哈希
    // ... 更多指令集

    // 大小核信息
    bool has_big_little;           // 是否为大小核架构
    uint32 big_core_count;         // 大核数量
    uint32 little_core_count;      // 小核数量
    uint32 big_core_max_freq;      // 大核最大频率(MHz)
    uint32 little_core_max_freq;   // 小核最大频率(MHz)

    // 缓存信息
    uint32 l1_data_cache_size;     // L1数据缓存(KB)
    uint32 l1_inst_cache_size;     // L1指令缓存(KB)
    uint32 l2_cache_size;          // L2缓存(KB)
    uint32 l3_cache_size;          // L3缓存(KB)
};
```

## 应用场景

### 1. 任务调度优化

```cpp
class TaskScheduler {
    const CpuInfo& cpu_info;

public:
    void ScheduleTask(Task* task) {
        if (cpu_info.arm.has_big_little) {
            if (task->IsComputeIntensive()) {
                // 分配给大核
                task->SetAffinity(GetBigCoreMask());
            } else {
                // 分配给小核
                task->SetAffinity(GetLittleCoreMask());
            }
        }
    }
};
```

### 2. 游戏引擎优化

```cpp
class GameEngine {
    void OptimizeForBigLittle(const CpuInfo& ci) {
        if (ci.arm.has_big_little) {
            // 物理计算分配给大核
            physics_thread.SetAffinity(ci.arm.big_core_count);

            // UI渲染分配给小核
            ui_thread.SetAffinity(ci.arm.little_core_count);

            // 根据电池状态动态调整
            if (IsLowBattery()) {
                ReduceBigCoreUsage();
            }
        }
    }
};
```

### 3. 服务器负载均衡

```cpp
class LoadBalancer {
    void DistributeWorkload(const CpuInfo& ci) {
        if (ci.arm.has_big_little) {
            // 高优先级任务 -> 大核
            // 低优先级任务 -> 小核
            // 后台任务 -> 小核
        }
    }
};
```

## 性能考虑

### 核间通信开销
- 大核和小核之间的数据同步可能产生额外开销
- 考虑任务的核间迁移成本

### 功耗管理
- 小核更省电，但性能较低
- 大核性能强，但功耗高
- 需要根据应用场景平衡使用

### 缓存一致性
- 不同核簇可能有不同的缓存配置
- 注意缓存一致性协议的开销

## 限制和注意事项

### Windows平台限制
- 当前实现主要基于Windows API
- ARM设备上的详细检测可能需要厂商特定的API

### 检测准确性
- 大小核检测基于启发式方法
- 在某些处理器上可能不完全准确
- 建议结合实际性能测试进行验证

### 跨平台兼容性
- 不同操作系统对ARM大小核的支持不同
- Windows ARM64支持相对有限
- Linux/Android平台有更好的支持

## 扩展建议

### 更精确的检测
```cpp
// 使用厂商特定API进行精确检测
bool DetectBigLittlePrecise() {
    // Qualcomm Kryo架构检测
    // Apple A系列检测
    // Samsung Mongoose/Cortex检测
    // 等
}
```

### 运行时频率监控
```cpp
// 实时监控核频率
class CoreMonitor {
    uint32 GetCurrentFrequency(int core_id);
    bool IsCoreBig(int core_id);
    void MonitorPowerConsumption();
};
```

### 智能调度器
```cpp
// 基于大小核的智能任务调度器
class SmartScheduler {
    void AnalyzeTaskRequirements(Task* task);
    void SelectOptimalCore(Task* task);
    void MigrateTaskIfNeeded(Task* task);
};
```

## 测试和验证

运行 `GetCpuInfo` 示例程序验证大小核检测：

```bash
cd Examples
.\GetCpuInfo.exe
```

输出示例：
```
=== CPU Information ===
CPU Count: 1
Core Count: 8
Logical Core Count: 8
Architecture: ARMv8

=== ARM CPU Features ===
...

=== Big.LITTLE Core Information ===
Has Big.LITTLE: Yes
Big Cores: 4
Little Cores: 4
Big Core Max Frequency: 2800 MHz
Little Core Max Frequency: 1800 MHz
```

## 总结

大小核检测功能为HGL库提供了现代移动处理器架构的支持。通过合理利用大核和小核的不同特性，应用程序可以实现更好的性能和功耗平衡。

关键优势：
- **自动检测**: 无需手动配置
- **跨平台**: 支持多种ARM处理器
- **易于使用**: 简单的API接口
- **可扩展**: 支持未来架构扩展