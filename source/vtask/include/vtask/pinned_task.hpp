
#pragma once

#include <vbase/core/noncopyable.hpp>
#include <vbase/core/pimpl.hpp>

#include <functional>

namespace vtask
{
    class PinnedTask final : private vbase::NonCopyable
    {
    public:
        using Fn = std::function<void()>;

        explicit PinnedTask(Fn fn);
        ~PinnedTask();

        void dependsOn(PinnedTask& other);

    private:
        friend class Scheduler;

        struct Impl;
        vbase::Pimpl<Impl> m_Impl;
    };
} // namespace vtask
