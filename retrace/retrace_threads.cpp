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

#include "retrace_threads.hpp"

namespace retrace {

RelayRace::RelayRace() {
    runners.push_back(new RelayRunner(this, 0));
}


RelayRace::~RelayRace() {
    assert(runners.size() >= 1);
    std::vector<RelayRunner*>::const_iterator it;
    for (it = runners.begin(); it != runners.end(); ++it) {
        RelayRunner* runner = *it;
        delete runner;
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
    Baton first;
    get_next_baton(first);
    if (!first.data) {
        /* Nothing to do */
        return;
    }

    RelayRunner *foreRunner = getForeRunner();
    if (first.thread_id == 0) {
        /* We are the forerunner thread, so no need to pass baton */
        foreRunner->baton = first;
    } else {
        passBaton(first);
    }

    /* Start the forerunner thread */
    foreRunner->runRace();
}


/**
 * Pass the baton (i.e., the call) to the appropriate thread.
 */
void
RelayRace::passBaton(Baton next) {
    if (0) std::cerr << "switching to thread " << next.thread_id << "\n";
    RelayRunner *runner = getRunner(next.thread_id);
    runner->receiveBaton(next);
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

RelayRunner::RelayRunner(RelayRace *race, unsigned _leg) :
    race(race),
    leg(_leg),
    finished(false),
    baton()
{
    /* The fore runner does not need a new thread */
    if (leg) {
        thread = std::thread(runnerThread, this);
    }
}

RelayRunner::~RelayRunner() {
    if (thread.joinable()) {
        thread.join();
    }
}

void
RelayRunner::runRace(void) {
    std::unique_lock<std::mutex> lock(mutex);

    while (1) {
        while (!finished && !baton.data) {
            wake_cond.wait(lock);
        }

        if (finished) {
            break;
        }

        assert(baton.data);
        Baton tmp_baton = baton;
        baton = {};

        runLeg(tmp_baton);
    }

    if (0) std::cerr << "leg " << leg << " actually finishing\n";

    if (leg == 0) {
        race->stopRunners();
    }
}

void
RelayRunner::runLeg(Baton current) {
    /* Consume successive calls for this thread. */
    do {
        assert(current.data);
        assert(current.thread_id == leg);
        race->run_baton(current);
        race->get_next_baton(current);
    } while (current.data && current.thread_id == leg);

    if (current.data) {
        /* Pass the baton */
        assert(current.thread_id != leg);
        race->flush();
        race->passBaton(current);
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

void
RelayRunner::receiveBaton(Baton new_baton) {
    assert(new_baton.thread_id == leg);

    mutex.lock();
    baton = new_baton;
    mutex.unlock();

    wake_cond.notify_one();
}

void
RelayRunner::finishRace() {
    if (0) std::cerr << "notify finish to leg " << leg << "\n";

    mutex.lock();
    finished = true;
    mutex.unlock();

    wake_cond.notify_one();
}

void
RelayRunner::runnerThread(RelayRunner *_this) {
    _this->runRace();
}

}
