/*! \file    Object.c
 *  \brief   Implementation of object data arrays.
 *  \author  Peter Chapin <keseymountain@spicacality.org>
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#if defined(__GLIBC__) || defined(__CYGWIN__)
#include <sys/sysinfo.h>
#endif

#include "global.h"
#include "Initialize.h"
#include "ThreadPool.h"

extern Object         *object_array;
extern ObjectDynamics *current_dynamics;
extern ObjectDynamics *next_dynamics;

extern ThreadPool pool;

struct Work_Unit {
    int start_index;
    int stop_index;
};

void *compute_next_dynamics(void *arg)
{
    struct Work_Unit *chunk = (struct Work_Unit *)arg;
    
    // For each object...
    for( int object_i = chunk->start_index; object_i < chunk->stop_index; ++object_i ) {
        Vector3 total_force = { 0.0, 0.0, 0.0 };

        // Consider interactions with all other objects...
        for( int object_j = 0; object_j < OBJECT_COUNT; ++object_j ) {
            if( object_i == object_j ) continue;

            Vector3 displacement = v3_subtract(
                current_dynamics[object_j].position, current_dynamics[object_i].position );
            double distance_squared = magnitude_squared( displacement );
            double distance = sqrt( distance_squared );
            double force_magnitude =
                ( G * object_array[object_i].mass * object_array[object_j].mass ) / distance_squared;
            Vector3 force = v3_multiply( (force_magnitude / distance ), displacement );
            total_force = v3_add( total_force, force );
        }

        // Total force on object_i is now known. Compute acceleration, velocity and position.
        Vector3 acceleration   = v3_divide( total_force, object_array[object_i].mass );
        Vector3 delta_v        = v3_multiply( TIME_STEP, acceleration );
        Vector3 delta_position = v3_multiply( TIME_STEP, current_dynamics[object_i].velocity );

        next_dynamics[object_i].velocity =
            v3_add( current_dynamics[object_i].velocity, delta_v );

        next_dynamics[object_i].position =
            v3_add( current_dynamics[object_i].position, delta_position );
    }
    return NULL;
}


void time_step( )
{
    #if defined(__GLIBC__) || defined(__CYGWIN__)
    int processor_count = get_nprocs( );
    #else
    int processor_count = pthread_num_processors_np( );
    #endif
    int objects_per_processor = OBJECT_COUNT / processor_count;

    struct Work_Unit *chunks =
        (struct Work_Unit *)malloc( processor_count * sizeof(struct Work_Unit) );
    threadid_t *thread_IDs =
        (threadid_t *)malloc( processor_count * sizeof(threadid_t) );
    
    // Create a thread for each CPU and set it working on its work unit.
    for( int i = 0; i < processor_count; ++i ) {
        struct Work_Unit *chunk =
            (struct Work_Unit *)malloc( sizeof( struct Work_Unit ) );
        chunk->start_index = i * objects_per_processor;
        if( i == processor_count - 1 )
            chunk->stop_index = OBJECT_COUNT;
        else
            chunk->stop_index   = ( i + 1 ) * objects_per_processor;
        thread_IDs[i] = ThreadPool_start( &pool, compute_next_dynamics, chunk);
    }
    

    // Wait for all threads to end.
    for( int i = 0; i < processor_count; ++i ) {
        ThreadPool_result( &pool, thread_IDs[i] );
    }
        
    // Swap the dynamics arrays.
    ObjectDynamics *temp = current_dynamics;
    current_dynamics     = next_dynamics;
    next_dynamics        = temp;

    free(chunks);
    free(thread_IDs);
}


void dump_dynamics( )
{
    for( int object_i = 0; object_i < OBJECT_COUNT; ++object_i ) {
        printf( "%4d: x = %11.3E, y = %11.3E, z = %11.3E\n", object_i,
                current_dynamics[object_i].position.x/AU,
                current_dynamics[object_i].position.y/AU,
                current_dynamics[object_i].position.z/AU );
    }
}
