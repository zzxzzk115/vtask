
#pragma once

#include "vtask/task_set.hpp"

#include <enkits/TaskScheduler.h>

#include <memory>
#include <vector>

namespace vtask
{
    struct TaskSet::Impl
    {
        Fn fn;

        std::vector<enki::Dependency> deps;

        std::unique_ptr<enki::TaskSet> task;

        Impl(uint32_t count, uint32_t grain, Fn f) : fn(std::move(f))
        {
            task = std::make_unique<enki::TaskSet>(
                count, [this](enki::TaskSetPartition r, uint32_t) { fn({r.start, r.end}); });

            task->m_MinRange = grain;
        }
    };

} // namespace vtask
