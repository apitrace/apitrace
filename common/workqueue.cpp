#include <queue>
#include <assert.h>

#include "os_workqueue.hpp"

namespace os
{

/**
 * return 0 on batch complete, -1 on thread exit request.
 */
int WorkQueue::run_tasks(void)
{
    os::unique_lock<os::mutex> lock(mutex);

    while (work_queue.empty() && !exit_workqueue) {
        wake_cond.wait(lock);
    }

    if (exit_workqueue) {
        return -1;
    }

    std::queue<WorkQueueWork *> batch;
    std::swap(work_queue, batch);
    busy = true;

    lock.unlock();

    assert(!batch.empty());
    while (!batch.empty()) {
        WorkQueueWork *task;

        task = batch.front();
        task->run();
        batch.pop();
        delete task;
    }

    lock.lock();

    busy = false;
    complete_cond.signal();

    return 0;
}

/* Must be called with WorkQueue::lock held */
void WorkQueue::wake_up_thread(void)
{
    wake_cond.signal();
}

void WorkQueue::queue_work(WorkQueueWork *task)
{
    mutex.lock();
    work_queue.push(task);
    wake_up_thread();
    mutex.unlock();
}

void WorkQueue::flush(void)
{
    os::unique_lock<os::mutex> lock(mutex);
    while (!work_queue.empty() || busy) {
        complete_cond.wait(lock);
    }
}

void WorkQueue::thread_entry(void)
{
    int err;

    do {
        err = run_tasks();
    } while (!err);
}

void WorkQueue::destroy(void)
{
    mutex.lock();
    exit_workqueue = true;
    wake_up_thread();
    mutex.unlock();
}

static
#ifdef _WIN32
DWORD WINAPI
#else
void *
#endif
WorkQueue__entry_thunk(void *data)
{
    WorkQueue *thread = static_cast<WorkQueue *>(data);

    thread->thread_entry();

#ifdef _WIN32
    return 0;
#else
    return NULL;
#endif
}

WorkQueue::WorkQueue(void) :
    busy(false),
    exit_workqueue(false),
    thread(WorkQueue__entry_thunk, this)
{
}

WorkQueue::~WorkQueue(void)
{
    thread.join();
}

}
