
#include "vtask/task_set.hpp"

#include "task_set_impl.hpp"

namespace vtask
{
    TaskSet::TaskSet(uint32_t count, uint32_t grain, Fn fn) : m_Impl(std::in_place, count, grain, std::move(fn)) {}

    TaskSet::~TaskSet() = default;

    void TaskSet::dependsOn(TaskSet& other)
    {
        auto& impl = *m_Impl;
        impl.deps.emplace_back();
        impl.task->SetDependency(impl.deps.back(), other.m_Impl->task.get());
    }
} // namespace vtask
