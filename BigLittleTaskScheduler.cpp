#include <hgl/platform/CpuInfo.h>
#include <hgl/platform/BigLittleDetector.h>
#include <iostream>
#include <thread>
#include <vector>
#include <functional>

using namespace hgl;
using namespace std;

/**
 * 简单的任务调度器示例
 * 演示如何基于大小核信息进行任务分配
 */
class TaskScheduler
{
private:
    CpuInfo cpu_info;
    vector<thread> threads;
    atomic<bool> running{true};

    // 任务队列
    vector<function<void()>> compute_tasks;
    vector<function<void()>> background_tasks;

    mutex task_mutex;

public:
    TaskScheduler()
    {
        if (!GetCpuInfo(&cpu_info)) {
            throw runtime_error("Failed to get CPU information");
        }
    }

    ~TaskScheduler()
    {
        Stop();
    }

    void Start()
    {
        BigLittleDetector::DetectionResult result = BigLittleDetector::Detect(cpu_info);

        cout << "Starting task scheduler..." << endl;
        cout << "Big.LITTLE detected: " << (result.has_big_little ? "Yes" : "No") << endl;

        if (result.has_big_little) {
            // 为大核创建计算线程
            for (uint32 i = 0; i < result.big_cores; ++i) {
                threads.emplace_back([this, i]() {
                    ComputeWorker(i);
                });
            }

            // 为小核创建后台线程
            for (uint32 i = 0; i < result.little_cores; ++i) {
                threads.emplace_back([this, i]() {
                    BackgroundWorker(i);
                });
            }
        } else {
            // 统一架构，为所有核创建通用线程
            for (uint32 i = 0; i < cpu_info.logical_core_count; ++i) {
                threads.emplace_back([this, i]() {
                    UnifiedWorker(i);
                });
            }
        }

        cout << "Created " << threads.size() << " worker threads" << endl;
    }

    void Stop()
    {
        running = false;

        for (auto& t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }

        threads.clear();
    }

    void AddComputeTask(function<void()> task)
    {
        lock_guard<mutex> lock(task_mutex);
        compute_tasks.push_back(move(task));
    }

    void AddBackgroundTask(function<void()> task)
    {
        lock_guard<mutex> lock(task_mutex);
        background_tasks.push_back(move(task));
    }

private:
    void ComputeWorker(int worker_id)
    {
        cout << "Compute worker " << worker_id << " started (Big core)" << endl;

        while (running) {
            function<void()> task;

            {
                lock_guard<mutex> lock(task_mutex);
                if (!compute_tasks.empty()) {
                    task = move(compute_tasks.back());
                    compute_tasks.pop_back();
                }
            }

            if (task) {
                cout << "Compute worker " << worker_id << " executing task..." << endl;
                task();
            } else {
                this_thread::sleep_for(chrono::milliseconds(10));
            }
        }

        cout << "Compute worker " << worker_id << " stopped" << endl;
    }

    void BackgroundWorker(int worker_id)
    {
        cout << "Background worker " << worker_id << " started (Little core)" << endl;

        while (running) {
            function<void()> task;

            {
                lock_guard<mutex> lock(task_mutex);
                if (!background_tasks.empty()) {
                    task = move(background_tasks.back());
                    background_tasks.pop_back();
                }
            }

            if (task) {
                cout << "Background worker " << worker_id << " executing task..." << endl;
                task();
            } else {
                this_thread::sleep_for(chrono::milliseconds(50)); // 更长的休眠，节省功耗
            }
        }

        cout << "Background worker " << worker_id << " stopped" << endl;
    }

    void UnifiedWorker(int worker_id)
    {
        cout << "Unified worker " << worker_id << " started" << endl;

        while (running) {
            function<void()> task;

            // 统一架构下，交替处理计算和后台任务
            {
                lock_guard<mutex> lock(task_mutex);
                if (!compute_tasks.empty()) {
                    task = move(compute_tasks.back());
                    compute_tasks.pop_back();
                } else if (!background_tasks.empty()) {
                    task = move(background_tasks.back());
                    background_tasks.pop_back();
                }
            }

            if (task) {
                cout << "Unified worker " << worker_id << " executing task..." << endl;
                task();
            } else {
                this_thread::sleep_for(chrono::milliseconds(20));
            }
        }

        cout << "Unified worker " << worker_id << " stopped" << endl;
    }
};

/**
 * 示例任务
 */
void HeavyComputationTask()
{
    // 模拟计算密集型任务
    volatile long long sum = 0;
    for (long long i = 0; i < 10000000; ++i) {
        sum += i * i;
    }
    this_thread::sleep_for(chrono::milliseconds(100));
}

void LightBackgroundTask()
{
    // 模拟轻量级后台任务
    this_thread::sleep_for(chrono::milliseconds(200));
}

int main()
{
    try {
        cout << "=== Big.LITTLE Task Scheduler Demo ===" << endl;

        TaskScheduler scheduler;

        // 显示CPU信息
        CpuInfo ci;
        if (GetCpuInfo(&ci)) {
            BigLittleDetector::DetectionResult result = BigLittleDetector::Detect(ci);

            cout << "CPU Architecture: ";
            switch(ci.arch) {
                case CpuArch::x86_64: cout << "x86_64"; break;
                case CpuArch::ARMv8: cout << "ARMv8"; break;
                case CpuArch::ARMv9: cout << "ARMv9"; break;
                default: cout << "Unknown"; break;
            }
            cout << endl;

            cout << "Big.LITTLE Support: " << (result.has_big_little ? "Yes" : "No") << endl;

            if (result.has_big_little) {
                cout << "Big cores: " << result.big_cores
                     << ", Little cores: " << result.little_cores << endl;
                cout << "Performance ratio: " << fixed << setprecision(2)
                     << result.performance_ratio << endl;
            }
        }

        // 启动调度器
        scheduler.Start();

        // 添加一些任务
        cout << "\nAdding tasks..." << endl;

        for (int i = 0; i < 5; ++i) {
            scheduler.AddComputeTask(HeavyComputationTask);
            scheduler.AddBackgroundTask(LightBackgroundTask);
        }

        // 运行一段时间
        cout << "Running tasks for 5 seconds..." << endl;
        this_thread::sleep_for(chrono::seconds(5));

        cout << "Stopping scheduler..." << endl;
        scheduler.Stop();

        cout << "Demo completed!" << endl;

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}