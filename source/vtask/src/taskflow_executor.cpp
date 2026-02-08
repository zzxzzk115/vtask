#include "vtask/backend/taskflow_executor.hpp"
#include "vtask/profiler.hpp"

#include <taskflow/taskflow.hpp>

namespace vtask
{
    struct TaskflowExecutor::Impl
    {
        explicit Impl(uint32_t threads) : exec(static_cast<size_t>(threads)) {}

        tf::Executor exec;
        IProfiler*   profiler {};
    };

    TaskflowExecutor::TaskflowExecutor(uint32_t threads) : m_Impl(new Impl(threads)) {}

    TaskflowExecutor::~TaskflowExecutor()
    {
        delete m_Impl;
        m_Impl = nullptr;
    }

    uint32_t TaskflowExecutor::concurrency() const noexcept
    {
        return static_cast<uint32_t>(m_Impl->exec.num_workers());
    }

    void TaskflowExecutor::setProfiler(IProfiler* profiler) noexcept { m_Impl->profiler = profiler; }

    IProfiler* TaskflowExecutor::profiler() const noexcept { return m_Impl->profiler; }

    void TaskflowExecutor::async(TaskFn fn, void* user, std::string_view name)
    {
        auto* const prof = m_Impl->profiler;

        m_Impl->exec.silent_async([=] {
            ProfileScope scope(prof, name.empty() ? "vtask::async" : name, ProfileCategory::eExecutor);
            fn(user);
        });
    }

    void TaskflowExecutor::parallelFor(uint32_t count, void* user, void (*fn)(uint32_t, void*), std::string_view name)
    {
        auto* const prof = m_Impl->profiler;

        tf::Taskflow tf;

        // Root scope for profiling
        auto root = tf.emplace(
            [=] { ProfileScope scope(prof, name.empty() ? "vtask::parallel_for" : name, ProfileCategory::eExecutor); });
        root.name("vtask_parallel_for_root");

        const uint32_t workers = static_cast<uint32_t>(m_Impl->exec.num_workers());

        const uint32_t grain = std::max<uint32_t>(1, count / std::max(1u, workers));

        std::vector<tf::Task> tasks;
        tasks.reserve(workers + 1);

        for (uint32_t begin = 0; begin < count; begin += grain)
        {
            const uint32_t end = std::min<uint32_t>(begin + grain, count);

            auto t = tf.emplace([=] {
                for (uint32_t i = begin; i < end; ++i)
                    fn(i, user);
            });

            t.name("vtask_parallel_for_chunk");
            root.precede(t);
            tasks.push_back(t);
        }

        m_Impl->exec.run(tf).wait();
    }

    void TaskflowExecutor::waitForAll() { m_Impl->exec.wait_for_all(); }

    void* TaskflowExecutor::nativeExecutor() noexcept { return &m_Impl->exec; }
} // namespace vtask
