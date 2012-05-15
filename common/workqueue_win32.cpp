#include <pthread.h>
#include <queue>
#include <assert.h>

#include "os.hpp"
#include "os_workqueue.hpp"

namespace os
{

/**
 * return 0 on batch complete, -1 on thread exit request.
 */
int WorkQueue::run_tasks(void)
{
    return 0;
}

void WorkQueue::queue_work(WorkQueueWork *task)
{
    task->run();
}

void WorkQueue::flush(void)
{
}

void WorkQueue::destroy(void)
{
}

WorkQueue::WorkQueue(void) :
	busy(false), exit_workqueue(false)
{
    static bool warned;

    if (!warned) {
        warned = true;
        os::log("%s: no workqueue implementation, running in single-threaded mode\n",
                __func__);
    }
}

WorkQueue::~WorkQueue(void)
{
}

}
