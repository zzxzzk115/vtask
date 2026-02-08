#pragma once

#include <cstdint>
#include <string_view>

namespace vtask
{
    class IProfiler;

    using TaskFn = void (*)(void* user);

    class IExecutor
    {
    public:
        virtual ~IExecutor() = default;

        virtual uint32_t concurrency() const noexcept = 0;

        virtual void       setProfiler(IProfiler* profiler) noexcept = 0;
        virtual IProfiler* profiler() const noexcept                 = 0;

        virtual void async(TaskFn fn, void* user, std::string_view name) = 0;

        virtual void
        parallelFor(uint32_t count, void* user, void (*fn)(uint32_t index, void* user), std::string_view name) = 0;

        virtual void waitForAll() = 0;
    };
} // namespace vtask
