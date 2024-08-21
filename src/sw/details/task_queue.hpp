#pragma once

#include <queue>
#include <functional>

namespace sw::details
{

struct Task
{
    std::function<void()> function;
};

class TaskQueue
{
public:
    void enqueue(const std::function<void()>& function)
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

} // sw::details
