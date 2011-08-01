/**************************************************************************
 *
 * Copyright 1999-2006 Brian Paul
 * Copyright 2008 Tungsten Graphics, Inc., Cedar Park, Texas.
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 **************************************************************************/


/**
 * @file
 *
 * Thread, mutex, condition variable.
 */


#ifndef OS_THREAD_H_
#define OS_THREAD_H_

#if defined _WIN32
#include <windows.h>
#else
#include <pthread.h> /* POSIX threads headers */
#include <stdio.h> /* for perror() */
#include <signal.h>
#endif

namespace OS {


#if defined _WIN32

typedef HANDLE Thread;
typedef CRITICAL_SECTION Mutex;

#define THREAD_ROUTINE WINAPI

static inline Thread ThreadCreate(void *(WINAPI * routine)(void *),
                                  void *param)
{
   DWORD id;
   return CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) routine,
                       param, 0, &id);
}

static inline int ThreadWait(Thread thread)
{
   if (WaitForSingleObject(thread, INFINITE) == WAIT_OBJECT_0)
      return 0;
   return -1;
}

static inline int ThreadDestroy(Thread thread)
{
   if (CloseHandle(thread))
      return 0;
   return -1;
}

/* http://locklessinc.com/articles/pthreads_on_windows/ */
#define MutexStatic(mutex) \
   static Mutex mutex = {(PCRITICAL_SECTION_DEBUG)-1, -1, 0, 0, 0, 0}

#define MutexInit(mutex) \
   InitializeCriticalSection(&mutex)

#define MutexDestroy(mutex) \
   DeleteCriticalSection(&mutex)

#define MutexLock(mutex) \
   EnterCriticalSection(&mutex)

#define MutexUnlock(mutex) \
   LeaveCriticalSection(&mutex)

#if 0

/*
 * CONDITION_VARIABLE is only available on newer versions of Windows
 * (Server 2008/Vista or later), but we care for XP.
 *
 * http://msdn.microsoft.com/en-us/library/ms682052(VS.85).aspx
 */

typedef CONDITION_VARIABLE Condvar;

#define CondvarStatic(cond) \
   /*static*/ Condvar cond = CONDITION_VARIABLE_INIT

#define CondvarInit(cond) \
   InitializeConditionVariable(&(cond))

#define CondvarDestroy(cond) \
   (void) cond /* nothing to do */

#define CondvarWait(cond, mutex) \
   SleepConditionVariableCS(&(cond), &(mutex), INFINITE)

#define CondvarSignal(cond) \
   WakeConditionVariable(&(cond))

#define CondvarBroadcast(cond) \
   WakeAllConditionVariable(&(cond))

#else

/*
 * See http://www.cs.wustl.edu/~schmidt/win32-cv-1.html
 * for potential pitfalls in implementation.
 */
typedef DWORD Condvar;

#define CondvarStatic(cond) \
   /*static*/ pipe_condvar cond = 1

#define CondvarInit(cond) \
   (void) (cond = 1)

#define CondvarDestroy(cond) \
   (void) cond

/* Poor man's pthread_cond_wait():
   Just release the mutex and sleep for one millisecond.
   The caller's while() loop does all the work. */
#define CondvarWait(cond, mutex) \
   do { MutexUnlock(mutex); \
        Sleep(cond); \
        MutexLock(mutex); \
   } while (0)

#define CondvarSignal(cond) \
   (void) cond

#define CondvarBroadcast(cond) \
   (void) cond

#endif

#else /* !_WIN32 */

typedef pthread_t Thread;
typedef pthread_mutex_t Mutex;
typedef pthread_cond_t Condvar;

#define THREAD_ROUTINE /* */

static inline Thread ThreadCreate(void *(* routine)( void *),
                                  void *param)
{
   Thread thread;
   sigset_t saved_set, new_set;
   int ret;

   /*
    * Block signals for new thread when spawning threads.
    */

   sigfillset(&new_set);
   pthread_sigmask(SIG_SETMASK, &new_set, &saved_set);
   ret = pthread_create( &thread, NULL, routine, param );
   pthread_sigmask(SIG_SETMASK, &saved_set, NULL);
   if (ret)
      return 0;
   return thread;
}

static inline int ThreadWait(Thread thread)
{
   return pthread_join(thread, NULL);
}

static inline int ThreadDestroy(Thread thread)
{
   return pthread_detach(thread);
}

#define MutexStatic(mutex) \
   static Mutex mutex = PTHREAD_MUTEX_INITIALIZER

#define MutexInit(mutex) \
   (void) pthread_mutex_init(&(mutex), NULL)

#define MutexDestroy(mutex) \
   pthread_mutex_destroy(&(mutex))

#define MutexLock(mutex) \
   (void) pthread_mutex_lock(&(mutex))

#define MutexUnlock(mutex) \
   (void) pthread_mutex_unlock(&(mutex))

#define CondvarStatic(mutex) \
   static Condvar mutex = PTHREAD_COND_INITIALIZER

#define CondvarInit(cond)	\
   pthread_cond_init(&(cond), NULL)

#define CondvarDestroy(cond) \
   pthread_cond_destroy(&(cond))

#define CondvarWait(cond, mutex) \
  pthread_cond_wait(&(cond), &(mutex))

#define CondvarSignal(cond) \
  pthread_cond_signal(&(cond))

#define CondvarBroadcast(cond) \
  pthread_cond_broadcast(&(cond))

#endif


}

#endif /* OS_THREAD_H_ */
