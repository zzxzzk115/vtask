#pragma once

#include "vtask/executor.hpp"

#include <cstdint>

namespace vtask
{
    class TaskflowExecutor final : public IExecutor
    {
    public:
        explicit TaskflowExecutor(uint32_t threads);
        ~TaskflowExecutor() override;

        uint32_t concurrency() const noexcept override;

        void       setProfiler(IProfiler* profiler) noexcept override;
        IProfiler* profiler() const noexcept override;

        void async(TaskFn fn, void* user, std::string_view name) override;

        void
        parallelFor(uint32_t count, void* user, void (*fn)(uint32_t index, void* user), std::string_view name) override;

        void waitForAll() override;

        // Taskflow-only escape hatch (forward declared in the .cpp).
        void* nativeExecutor() noexcept;

    private:
        struct Impl;
        Impl* m_Impl {};
    };
} // namespace vtask
