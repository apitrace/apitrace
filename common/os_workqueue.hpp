#ifndef _OS_WORKQUEUE_HPP_
#define _OS_WORKQUEUE_HPP_

#include <queue>

#include "os_thread.hpp"

namespace os {


class WorkQueue;

class WorkQueueWork {
protected:
    friend class WorkQueue;

public:
    virtual void run(void) = 0;
    virtual ~WorkQueueWork(void) { }
};

class WorkQueue {
    std::queue<WorkQueueWork *> work_queue;

    bool busy;
    bool exit_workqueue;
    os::condition_variable wake_cond;
    os::condition_variable complete_cond;

    os::mutex mutex;

    os::thread thread;

    void wake_up_thread(void);
    int run_tasks(void);
public:
    void thread_entry(void);
    void queue_work(WorkQueueWork *work);
    void flush(void);
    void destroy(void);

    WorkQueue(void);
    ~WorkQueue();
};

}

#endif
