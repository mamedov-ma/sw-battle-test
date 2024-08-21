#pragma once

#include <functional>
#include <queue>

namespace sw::details
{

struct Task
{
    std::function<void()> function;
};

class TaskQueue
{
public:
    void enqueue(std::function<void()> const& function)
    {
        tasks.push({function});
    }

    void process()
    {
        while (!tasks.empty())
        {
            Task task = tasks.front();
            tasks.pop();
            task.function();
        }
    }

private:
    std::queue<Task> tasks;
};

} // namespace sw::details
