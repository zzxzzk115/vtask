
#pragma once

#include "vtask/scheduler.hpp"

#include <enkits/TaskScheduler.h>

#include <algorithm>
#include <thread>

namespace vtask
{
    struct Scheduler::Impl
    {
        enki::TaskScheduler scheduler;

        Impl(uint32_t threads)
        {
            auto cfg = scheduler.GetConfig();

            if (threads == 0)
                threads = std::max(1u, static_cast<uint32_t>(std::thread::hardware_concurrency()));

            cfg.numTaskThreadsToCreate = threads > 0 ? threads - 1 : 0;

            scheduler.Initialize(cfg);
        }

        ~Impl() { scheduler.WaitforAllAndShutdown(); }
    };
} // namespace vtask
