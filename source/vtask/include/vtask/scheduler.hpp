
#pragma once

#include <vbase/core/noncopyable.hpp>
#include <vbase/core/pimpl.hpp>

#include <cstdint>

namespace vtask
{
    class TaskSet;
    class PinnedTask;

    class Scheduler final : private vbase::NonCopyable
    {
    public:
        explicit Scheduler(uint32_t threads = 0);
        ~Scheduler();

        void run(TaskSet& task);
        void run(PinnedTask& task);

        void wait(TaskSet& task);
        void wait(PinnedTask& task);

        void waitAll();

        uint32_t concurrency() const noexcept;

    private:
        struct Impl;
        vbase::Pimpl<Impl> m_Impl;
    };
} // namespace vtask
