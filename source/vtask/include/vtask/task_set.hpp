
#pragma once

#include <vbase/core/noncopyable.hpp>
#include <vbase/core/pimpl.hpp>

#include <cstdint>
#include <functional>

namespace vtask
{
    struct Range
    {
        uint32_t begin;
        uint32_t end;
    };

    class TaskSet final : private vbase::NonCopyable
    {
    public:
        using Fn = std::function<void(Range)>;

        TaskSet(uint32_t count, uint32_t grain, Fn fn);
        ~TaskSet();

        void dependsOn(TaskSet& other);

    private:
        friend class Scheduler;

        struct Impl;
        vbase::Pimpl<Impl> m_Impl;
    };
} // namespace vtask
