/**************************************************************************
 *
 * Copyright 2011 Jose Fonseca
 * Copyright (C) 2013 Intel Corporation. All rights reversed.
 * Author: Shuang He <shuang.he@intel.com>
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 **************************************************************************/


#include <cassert>
#include <iostream>

#include "os_thread.hpp"
#include "relay_race.hpp"
#include "retrace.hpp"
#include "trace_model.hpp"
#include "trace_parser.hpp"


namespace retrace {
void retraceCall(trace::Call *call);
void flushRendering();
}

static trace::ParseBookmark lastFrameStart;


/**
 * Each runner is a thread.
 *
 * The fore runner doesn't have its own thread, but instead uses the thread
 * where the race started.
 */
class RelayRunner
{
private:
    friend class RelayRace;

    RelayRace *race;

    unsigned leg;
    
    os::mutex mutex;
    os::condition_variable wake_cond;

    /**
     * There are protected by the mutex.
     */
    bool finished;
    trace::Call *baton;

    os::thread thread;

    static void
    runnerThread(RelayRunner *_this);

public:
    RelayRunner(RelayRace *race, unsigned _leg) :
        race(race),
        leg(_leg),
        finished(false),
        baton(0)
    {
        /* The fore runner does not need a new thread */
        if (leg) {
            thread = os::thread(runnerThread, this);
        }
    }

    ~RelayRunner() {
        if (thread.joinable()) {
            thread.join();
        }
    }

    /**
     * Thread main loop.
     */
    void
    runRace(void) {
        os::unique_lock<os::mutex> lock(mutex);

        while (1) {
            while (!finished && !baton) {
                wake_cond.wait(lock);
            }

            if (finished) {
                break;
            }

            assert(baton);
            trace::Call *call = baton;
            baton = 0;

            runLeg(call);
        }

        if (0) std::cerr << "leg " << leg << " actually finishing\n";

        if (leg == 0) {
            race->stopRunners();
        }
    }

    /**
     * Interpret successive calls.
     */
    void
    runLeg(trace::Call *call) {

        /* Consume successive calls for this thread. */
        do {
            bool callEndsFrame = false;
            static trace::ParseBookmark frameStart;

            assert(call);
            assert(call->thread_id == leg);

            if (retrace::loopCount && call->flags & trace::CALL_FLAG_END_FRAME) {
                callEndsFrame = true;
                retrace::parser.getBookmark(frameStart);
            }

            retrace::retraceCall(call);
            delete call;
            call = retrace::parser.parse_call();

            /* Restart last frame if looping is requested. */
            if (retrace::loopCount) {
                if (!call) {
                    retrace::parser.setBookmark(lastFrameStart);
                    call = retrace::parser.parse_call();
                    if (retrace::loopCount > 0) {
                        --retrace::loopCount;
                    }
                } else if (callEndsFrame) {
                    lastFrameStart = frameStart;
                }
            }

        } while (call && call->thread_id == leg);

        if (call) {
            /* Pass the baton */
            assert(call->thread_id != leg);
            retrace::flushRendering();
            race->passBaton(call);
        } else {
            /* Reached the finish line */
            if (0) std::cerr << "finished on leg " << leg << "\n";
            if (leg) {
                /* Notify the fore runner */
                race->finishLine();
            } else {
                /* We are the fore runner */
                finished = true;
            }
        }
    }

    /**
     * Called by other threads when relinquishing the baton.
     */
    void
    receiveBaton(trace::Call *call) {
        assert (call->thread_id == leg);

        mutex.lock();
        baton = call;
        mutex.unlock();

        wake_cond.notify_one();
    }

    /**
     * Called by the fore runner when the race is over.
     */
    void
    finishRace() {
        if (0) std::cerr << "notify finish to leg " << leg << "\n";

        mutex.lock();
        finished = true;
        mutex.unlock();

        wake_cond.notify_one();
    }
};


void
RelayRunner::runnerThread(RelayRunner *_this) {
    _this->runRace();
}


RelayRace::RelayRace() {
    runners.push_back(new RelayRunner(this, 0));
}


RelayRace::~RelayRace() {
    assert(runners.size() >= 1);
    std::vector<RelayRunner*>::const_iterator it;
    for (it = runners.begin(); it != runners.end(); ++it) {
        RelayRunner* runner = *it;
        if (runner) {
            delete runner;
        }
    }
}


/**
 * Get (or instantiate) a runner for the specified leg.
 */
RelayRunner *
RelayRace::getRunner(unsigned leg) {
    RelayRunner *runner;

    if (leg >= runners.size()) {
        runners.resize(leg + 1);
        runner = 0;
    } else {
        runner = runners[leg];
    }
    if (!runner) {
        runner = new RelayRunner(this, leg);
        runners[leg] = runner;
    }
    return runner;
}


/**
 * Start the race.
 */
void
RelayRace::run(void) {
    trace::Call *call;
    call = retrace::parser.parse_call();
    if (!call) {
        /* Nothing to do */
        return;
    }

    /* If the user wants to loop we need to get a bookmark target. We
     * usually get this after replaying a call that ends a frame, but
     * for a trace that has only one frame we need to get it at the
     * beginning. */
    if (retrace::loopCount) {
        retrace::parser.getBookmark(lastFrameStart);
    }

    RelayRunner *foreRunner = getForeRunner();
    if (call->thread_id == 0) {
        /* We are the forerunner thread, so no need to pass baton */
        foreRunner->baton = call;
    } else {
        passBaton(call);
    }

    /* Start the forerunner thread */
    foreRunner->runRace();
}


/**
 * Pass the baton (i.e., the call) to the appropriate thread.
 */
void
RelayRace::passBaton(trace::Call *call) {
    if (0) std::cerr << "switching to thread " << call->thread_id << "\n";
    RelayRunner *runner = getRunner(call->thread_id);
    runner->receiveBaton(call);
}


/**
 * Called when a runner other than the forerunner reaches the finish line.
 *
 * Only the fore runner can finish the race, so inform him that the race is
 * finished.
 */
void
RelayRace::finishLine(void) {
    RelayRunner *foreRunner = getForeRunner();
    foreRunner->finishRace();
}


/**
 * Called by the fore runner after finish line to stop all other runners.
 */
void
RelayRace::stopRunners(void) {
    std::vector<RelayRunner*>::const_iterator it;
    for (it = runners.begin() + 1; it != runners.end(); ++it) {
        RelayRunner* runner = *it;
        if (runner) {
            runner->finishRace();
        }
    }
}
