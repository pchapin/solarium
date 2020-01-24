/*!
 * \file    ThreadPool.h
 * \brief   Interface to a thread pool type.
 * \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
 *
 * The thread pool defined here uses POSIX threads as the underlying thread API.
 */

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>
#include <semaphore.h>

#ifdef __cplusplus
extern "C" {
#endif

// ThreadPool class
// ================

struct ThreadInformation;

//! Manages a pool of pre-created threads. It is permitted to have multiple pool objects.
typedef struct {
    int    pool_size;         // Number of threads in the pool.
    sem_t  worker_count;      // Number of available workers.
    struct ThreadInformation *thread_information; // Points at dynamic array of ThreadInformation.
} ThreadPool;

//! Type used to hold thread ID values in the scope of a particular pool.
typedef int threadid_t;

//! Initializes the thread pool pointed at by 'self.'
void ThreadPool_initialize( ThreadPool *self );

//! Cleans up the thread pool pointed at by 'self.'
/*!
 * The pool kills all threads immediately regardless of what they are doing. Users should be
 * sure to collect the work result from every thread before destroying the pool to be sure that
 * all threads have completed their tasks.
 */
void ThreadPool_destroy( ThreadPool *self );

//! Return the total number of threads in the pool (not just the number of available threads).
int ThreadPool_count( ThreadPool *self );

//! Directs a thread from the pool to start some work.
/*!
 * Threads that have completed but that have not yet had their work result picked up are not
 * available for more work. Do not call this function more times than there are threads in the
 * pool without picking up the work result from previous tasks first. If you do so the calling
 * thread will block indefinitely.
 *
 * \param work_function Pointer to a function the thread is to execute.
 * \param arg Pointer to an object containing the thread argument(s).
 * \return The pool-relative ID number of the thread used to process this unit of work.
 */
threadid_t ThreadPool_start( ThreadPool *self, void *( *work_function )( void * ), void *arg );

//! Pick up the work result from a particular thread.
/*!
 * The user should pick up the work result of every unit of work dispatched before destroying
 * the pool to ensure that threads are not canceled before their tasks are completed. Even if
 * the expected result is NULL, this function should still be called to retrieve it.
 *
 * \param ID The pool-relative ID number of the thread from which the last work result is
 * needed. If that thread has not completed its task (or has never been given a task) this
 * function suspends until a new work result is ready.
 *
 * \return The work result as returned by the thread function.
 */
void *ThreadPool_result( ThreadPool *self, threadid_t ID );

#ifdef __cplusplus
}
#endif

#endif
