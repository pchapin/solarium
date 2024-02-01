/*! \file    main.c
 *  \brief   Main program of the parallel solar system simulator (barrier version).
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#if defined(__GLIBC__) || defined(__CYGWIN__)
#include <sys/sysinfo.h>
#endif
#include "global.h"
#include "initialize.h"
#include "Timer.h"

#define STEPS_PER_YEAR 8766  // Number of hours in a year.

//! Compute the next dynamics from the current dynamics.
/*!
 * This function takes one step of simulated time.
 *
 * \param start_index The object ID of the first object managed by this thread.
 * \param end_index The object ID just past the last object managed by this thread.
 */
void time_step( int start_index, int end_index );

struct TaskDescriptor {
    int start_index;  // Object ID at the start of thread's work space.
    int end_index;    // Object ID at the end of thread's work space.
    int step_count;   // Number of steps the thread should take.
};

pthread_barrier_t step_barrier;  // Used to synchronize threads while stepping.
pthread_barrier_t swap_barrier;  // Used to synchronize threads after dynamics swapping.
long long total_steps = 0;  // Total number of steps executed so far.
int       total_years = 0;  // Total number of years simulated so far.


void *thread_function( void *arg )
{
    ObjectDynamics *temp;
    struct TaskDescriptor *task = (struct TaskDescriptor *)arg;

    for( int i = 0; i < task->step_count; ++i ) {
        time_step( task->start_index, task->end_index );
        if( pthread_barrier_wait( &step_barrier ) == PTHREAD_BARRIER_SERIAL_THREAD ) {
            total_steps++;

            // Print out a message after 100 steps just to give the user something to see.
            if( total_steps % 100 == 0 )
                fprintf( stderr, "STEP %4lld\n", total_steps );

            if( total_steps % STEPS_PER_YEAR == 0 ) {
                total_years++;
                if( total_years % 10 == 0 ) {
                    fprintf( stderr, "Years simulated = %d\r", total_years );
                    fflush( stderr );
                }
            }
            // Swap the dynamics arrays.
            temp             = current_dynamics;
            current_dynamics = next_dynamics;
            next_dynamics    = temp;
        }
        pthread_barrier_wait( &swap_barrier );
    }
    free( task );
    return NULL;
}


int main( int argc, char **argv )
{
    Timer stopwatch;
    int   return_code = EXIT_SUCCESS;

    #if defined(__GLIBC__) || defined(__CYGWIN__)
    int processor_count = get_nprocs( );
    #else
    int processor_count = pthread_num_processors_np( );
    #endif

    int objects_per_processor = OBJECT_COUNT / processor_count;
    pthread_t *thread_IDs;

    printf( "%d processing elements detected!\n", processor_count );

    initialize_object_arrays( );
    Timer_initialize( &stopwatch );
    printf( "START position\n" );
    dump_dynamics( );
    Timer_start( &stopwatch );

    pthread_barrier_init( &step_barrier, NULL, processor_count );
    pthread_barrier_init( &swap_barrier, NULL, processor_count );
    thread_IDs = (pthread_t *)malloc( processor_count * sizeof(pthread_t) );

    // Create a thread for each CPU and set it working on its work unit.
    for( int i = 0; i < processor_count; ++i ) {
        struct TaskDescriptor *task =
            (struct TaskDescriptor *)malloc( sizeof( struct TaskDescriptor ) );
        task->step_count = STEPS_PER_YEAR * 1;
        task->start_index = i * objects_per_processor;
        if( i == processor_count - 1 )
            task->end_index = OBJECT_COUNT;
        else
            task->end_index   = ( i + 1 ) * objects_per_processor;
        pthread_create( &thread_IDs[i], NULL, thread_function, task);
    }

    // Wait for all thread to end.
    for( int i = 0; i < processor_count; ++i ) {
        pthread_join( thread_IDs[i], NULL );
    }

    free( thread_IDs );
    pthread_barrier_destroy( &step_barrier );
    pthread_barrier_destroy( &swap_barrier );

    Timer_stop( &stopwatch );
    printf( "\nEND position\n" );
    dump_dynamics( );
    printf( "Time elapsed = %ld milliseconds\n", Timer_time( &stopwatch ) );

    return return_code;
}
