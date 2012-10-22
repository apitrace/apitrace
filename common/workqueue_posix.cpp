#include <pthread.h>
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
    lock.lock();

    while (work_queue.empty() && !exit_workqueue)
        pthread_cond_wait(&wake_cond, &lock.native_handle());

    if (exit_workqueue) {
        lock.unlock();
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
    pthread_cond_signal(&complete_cond);

    lock.unlock();

    return 0;
}

/* Must be called with WorkQueue::lock held */
void WorkQueue::wake_up_thread(void)
{
    pthread_cond_signal(&wake_cond);
}

void WorkQueue::queue_work(WorkQueueWork *task)
{
    lock.lock();
    work_queue.push(task);
    wake_up_thread();
    lock.unlock();
}

void WorkQueue::flush(void)
{
    lock.lock();
    while (!work_queue.empty() || busy)
        pthread_cond_wait(&complete_cond, &lock.native_handle());
    lock.unlock();
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
    lock.lock();
    exit_workqueue = true;
    wake_up_thread();
    lock.unlock();
}

extern "C"
void *WorkQueue__entry_thunk(void *data)
{
    WorkQueue *thread = static_cast<WorkQueue *>(data);

    thread->thread_entry();

    return NULL;
}

WorkQueue::WorkQueue(void) :
    busy(false), exit_workqueue(false)
{
    int err;

    pthread_cond_init(&wake_cond, NULL);
    pthread_cond_init(&complete_cond, NULL);
    err = pthread_create(&handle, NULL, WorkQueue__entry_thunk, this);
    assert(!err);
}

WorkQueue::~WorkQueue(void)
{
    pthread_join(handle, NULL);
}

}
