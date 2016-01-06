/**************************************************************************
 *
 * Copyright 2016 VMware, Inc.
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


#include "os_thread.hpp"

#include "gtest/gtest.h"

TEST(os_thread, hardware_concurrency)
{
    EXPECT_GT(os::thread::hardware_concurrency(), 0);
}


static bool b0;
static void f0(void) {
    b0 = true;
}

static bool b1;
static void f1(int a1) {
    b1 = a1 == 1;
}

static bool b2;
static void f2(int a2, int a3) {
    b2 = a2 == 2 && a3 == 3;
}

TEST(os_thread, thread)
{
    os::thread t0(&f0);
    t0.join();
    EXPECT_TRUE(b0);

    os::thread t1(&f1, 1);
    t1.join();
    EXPECT_TRUE(b1);

    os::thread t2(&f2, 2, 3);
    t2.join();
    EXPECT_TRUE(b2);
}

#define NUM_THREADS 4


struct Data
{
    bool cvb[NUM_THREADS];
    os::condition_variable cv;
    os::mutex mutex;
    volatile unsigned c;
};


static void cvf(Data *data, unsigned idx)
{
    std::cerr << idx << " started" << std::endl;

    {
        os::unique_lock<os::mutex> l(data->mutex);
        data->c += 1;
    }

    os::mutex mutex;
    os::unique_lock<os::mutex> lock(mutex);
    std::cerr << idx << " waiting..." << std::endl;
    data->cv.wait(lock);
    data->cvb[idx] = true;
    std::cerr << idx << " finished." << std::endl;
}


TEST(os_thread, cv)
{
    os::thread t[NUM_THREADS];
    Data data;
    unsigned i;

    data.c = 0;
    for (i = 0; i < NUM_THREADS; ++i) {
        data.cvb[i] = false;
        t[i] = os::thread(cvf, &data, i);
    }

    for (i = 0; i < NUM_THREADS; ++i) {
        EXPECT_FALSE(data.cvb[i]);
    }

    std::cerr << "waiting..." << std::endl;

    do {
        os::unique_lock<os::mutex> l(data.mutex);
        if (data.c == NUM_THREADS) {
            break;
        }
    } while(true);

    std::cerr << "notifying..." << std::endl;
    data.cv.notify_all();

    std::cerr << "joining" << std::endl;
    for (i = 0; i < NUM_THREADS; ++i) {
        t[i].join();
        EXPECT_TRUE(data.cvb[i]);
    }
}


int
main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
