/**************************************************************************
 *
 * Copyright (c) 2012 Jakob Progsch, Václav Zeman
 * Copyright (c) 2015 Emmanuel Gil Peyrot, Collabora Ltd.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 *    1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 *    2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 *    3. This notice may not be removed or altered from any source
 *    distribution.
 *
 **************************************************************************/


#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <queue>
#include <vector>

#include "os_thread.hpp"


class ThreadPool {
public:
    ThreadPool(size_t);
    template<class F, class... Args>
    void enqueue(F&& f, Args&&... args);
    ~ThreadPool();
private:
    // need to keep track of threads so we can join them
    std::vector<os::thread> workers;
    // the task queue
    std::queue<std::function<void()>> tasks;

    // synchronization
    os::mutex queue_mutex;
    os::condition_variable condition;
    bool stop;
};


// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool(size_t threads)
    :   stop(false)
{
    for(size_t i = 0;i<threads;++i)
        workers.emplace_back(
            [this]
            {
                for(;;)
                {
                    std::function<void()> task;

                    {
                        os::unique_lock<os::mutex> lock(this->queue_mutex);
                        this->condition.wait(lock,
                            [this]{ return this->stop || !this->tasks.empty(); });
                        if(this->stop && this->tasks.empty())
                            return;
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }

                    task();
                }
            }
        );
}

// add new work item to the pool
template<class F, class... Args>
void ThreadPool::enqueue(F&& f, Args&&... args)
{
    auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);

    {
        os::unique_lock<os::mutex> lock(queue_mutex);

        // don't allow enqueueing after stopping the pool
        assert(!stop);

        tasks.emplace(task);
    }
    condition.notify_one();
}

// the destructor joins all threads
inline ThreadPool::~ThreadPool()
{
    {
        os::unique_lock<os::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for(os::thread &worker: workers)
        worker.join();
}
