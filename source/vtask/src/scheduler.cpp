#include "pinned_task_impl.hpp"
#include "scheduler_impl.hpp"
#include "task_set_impl.hpp"

namespace vtask
{
    Scheduler::Scheduler(uint32_t threads) : m_Impl(std::in_place, threads) {}

    Scheduler::~Scheduler() = default;

    void Scheduler::run(TaskSet& task) { m_Impl->scheduler.AddTaskSetToPipe(task.m_Impl->task.get()); }

    void Scheduler::run(PinnedTask& task) { m_Impl->scheduler.AddPinnedTask(task.m_Impl.get()); }

    void Scheduler::wait(TaskSet& task) { m_Impl->scheduler.WaitforTask(task.m_Impl->task.get()); }

    void Scheduler::wait(PinnedTask& task) { m_Impl->scheduler.WaitforTask(task.m_Impl.get()); }

    void Scheduler::waitAll() { m_Impl->scheduler.WaitforAll(); }

    uint32_t Scheduler::concurrency() const noexcept
    {
        auto cfg = m_Impl->scheduler.GetConfig();
        return cfg.numTaskThreadsToCreate + 1;
    }
} // namespace vtask
