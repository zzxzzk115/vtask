
#include "vtask/pinned_task.hpp"

#include "pinned_task_impl.hpp"

namespace vtask
{
    PinnedTask::PinnedTask(Fn fn) : m_Impl(std::in_place, std::move(fn)) {}

    PinnedTask::~PinnedTask() = default;

    void PinnedTask::dependsOn(PinnedTask& other)
    {
        auto& impl = *m_Impl;
        impl.deps.emplace_back();
        impl.SetDependency(impl.deps.back(), other.m_Impl.get());
    }
} // namespace vtask
