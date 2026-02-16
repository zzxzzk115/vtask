
#pragma once

#include "vtask/pinned_task.hpp"

#include <enkits/TaskScheduler.h>
#include <vector>

namespace vtask
{
    struct PinnedTask::Impl : enki::IPinnedTask
    {
        Fn                            fn;
        std::vector<enki::Dependency> deps;

        explicit Impl(Fn f) : fn(std::move(f)) {}

        void Execute() override { fn(); }
    };
} // namespace vtask
