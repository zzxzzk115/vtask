#include "vtask/backend/taskflow_task_graph.hpp"
#include "vtask/backend/taskflow_executor.hpp"
#include "vtask/profiler.hpp"

#include <taskflow/taskflow.hpp>

#include <string>
#include <vector>

namespace vtask
{
    struct TaskflowTaskGraph::Impl
    {
        tf::Taskflow          tf;
        std::vector<tf::Task> tasks;

        IProfiler* runProfiler {};
    };

    TaskflowTaskGraph::TaskflowTaskGraph() : m_Impl(new Impl()) {}

    TaskflowTaskGraph::~TaskflowTaskGraph()
    {
        delete m_Impl;
        m_Impl = nullptr;
    }

    ITaskGraph::Node TaskflowTaskGraph::emplace(TaskFn fn, void* user, std::string_view name)
    {
        const uint32_t id = static_cast<uint32_t>(m_Impl->tasks.size());

        auto t = m_Impl->tf.emplace([=, this] {
            ProfileScope scope(m_Impl->runProfiler, name.empty() ? "vtask::task" : name, ProfileCategory::eTaskGraph);
            fn(user);
        });

        if (!name.empty())
            t.name(std::string(name));

        m_Impl->tasks.push_back(t);
        return Node {id};
    }

    void TaskflowTaskGraph::precede(Node a, Node b) { m_Impl->tasks[a.id].precede(m_Impl->tasks[b.id]); }

    void TaskflowTaskGraph::runAndWait(IExecutor& exec, std::string_view name)
    {
        // Taskflow-only backend: require TaskflowExecutor.
        auto* tfexec = dynamic_cast<TaskflowExecutor*>(&exec);
        if (!tfexec)
            return;

        auto* native = static_cast<tf::Executor*>(tfexec->nativeExecutor());

        m_Impl->runProfiler = exec.profiler();

        if (!name.empty())
        {
            ProfileScope scope(m_Impl->runProfiler, name, ProfileCategory::eTaskGraph);
            native->run(m_Impl->tf).wait();
        }
        else
        {
            native->run(m_Impl->tf).wait();
        }

        m_Impl->runProfiler = nullptr;
    }
} // namespace vtask
