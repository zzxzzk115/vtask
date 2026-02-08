#include <vtask/backend/taskflow_executor.hpp>
#include <vtask/backend/taskflow_task_graph.hpp>
#include <vtask/profiler.hpp>

#include <iostream>
#include <thread>
#include <vector>

using namespace vtask;

// -------------------------------------------------------------
// Simple in-memory profiler just prints events
// -------------------------------------------------------------
class ConsoleProfiler : public IProfiler
{
public:
    void onEvent(const ProfileEvent& e) noexcept override
    {
        const char* phase = e.begin ? "BEGIN" : "END";
        std::cout << "[" << phase << "] " << e.name << " (thread=" << e.threadId << ")\n";
    }

    uint32_t threadId() noexcept override
    {
        return static_cast<uint32_t>(std::hash<std::thread::id> {}(std::this_thread::get_id()));
    }
};

// -------------------------------------------------------------
// Example workload
// -------------------------------------------------------------
struct Data
{
    std::vector<int> values;
};

// task functions must match: void(void*)
void init_task(void* u)
{
    auto* d = static_cast<Data*>(u);
    for (auto& v : d->values)
        v = 1;
}

void multiply_task(void* u)
{
    auto* d = static_cast<Data*>(u);
    for (auto& v : d->values)
        v *= 2;
}

// parallel body: void(uint32_t, void*)
void add_index(uint32_t i, void* u)
{
    auto* d = static_cast<Data*>(u);
    d->values[i] += static_cast<int>(i);
}

// -------------------------------------------------------------
// main
// -------------------------------------------------------------
int main()
{
    const uint32_t threads = std::max(1u, std::thread::hardware_concurrency());

    TaskflowExecutor exec(threads);

    ConsoleProfiler profiler;
    exec.setProfiler(&profiler);

    // Prepare data
    Data data;
    data.values.resize(1000);

    // -----------------------------
    // 1) Task Graph example
    // -----------------------------
    {
        TaskflowTaskGraph graph;

        auto a = graph.emplace(init_task, &data, "Init");
        auto b = graph.emplace(multiply_task, &data, "Multiply");

        graph.precede(a, b);

        graph.runAndWait(exec, "InitPipeline");
    }

    // -----------------------------
    // 2) ParallelFor example
    // -----------------------------
    exec.parallelFor(static_cast<uint32_t>(data.values.size()), &data, add_index, "AddIndex");

    exec.waitForAll();

    // -----------------------------
    // Verify result
    // -----------------------------
    int64_t sum = 0;
    for (auto v : data.values)
        sum += v;

    std::cout << "Checksum: " << sum << "\n";
}
