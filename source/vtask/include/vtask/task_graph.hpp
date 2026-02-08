#pragma once

#include <cstdint>
#include <string_view>

namespace vtask
{
    class IExecutor;

    using TaskFn = void (*)(void* user);

    class ITaskGraph
    {
    public:
        struct Node
        {
            uint32_t id {};
        };

        virtual ~ITaskGraph() = default;

        virtual Node emplace(TaskFn fn, void* user, std::string_view name) = 0;
        virtual void precede(Node a, Node b)                               = 0;

        virtual void runAndWait(IExecutor& exec, std::string_view name) = 0;
    };
} // namespace vtask
