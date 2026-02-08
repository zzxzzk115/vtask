#pragma once

#include "vtask/task_graph.hpp"

#include <string_view>

namespace vtask
{
    class TaskflowTaskGraph final : public ITaskGraph
    {
    public:
        TaskflowTaskGraph();
        ~TaskflowTaskGraph() override;

        Node emplace(TaskFn fn, void* user, std::string_view name) override;
        void precede(Node a, Node b) override;

        void runAndWait(IExecutor& exec, std::string_view name) override;

    private:
        struct Impl;
        Impl* m_Impl {};
    };
} // namespace vtask
