/*! \file    Object.c
 *  \brief   Implementation of object data arrays.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <stdlib.h>
#if  defined(__GLIBC__) || defined(__CYGWIN__)
#include <sys/sysinfo.h>
#endif

#include "global.h"
#include "Initialize.h"

struct WorkUnit {
    int start_index;
    int stop_index;
};

void *compute_next_dynamics(void *arg)
{
    struct WorkUnit *chunk = (struct WorkUnit *)arg;

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
    #elif defined(__APPLE__)
    int processor_count = get_macOS_nprocs( );
    #else
    int processor_count = pthread_num_processors_np( );
    #endif
    int objects_per_processor = OBJECT_COUNT / processor_count;

    struct WorkUnit *chunks =
        (struct WorkUnit *)malloc( processor_count * sizeof(struct WorkUnit) );
    pthread_t *thread_IDs =
        (pthread_t *)malloc( processor_count * sizeof(pthread_t) );

    // Split the problem into chunks.
    for( int i = 0; i < processor_count; ++i ) {
        chunks[i].start_index = i * objects_per_processor;
        chunks[i].stop_index = ( i + 1 ) * objects_per_processor;
    }
    chunks[processor_count - 1].stop_index = OBJECT_COUNT;

    // Create a thread for each CPU and set it working on its work unit.
    for( int i = 0; i < processor_count; ++i ) {
        pthread_create( &thread_IDs[i], NULL, compute_next_dynamics, &chunks[i]);
    }


    // Wait for all thread to end.
    for( int i = 0; i < processor_count; ++i ) {
        pthread_join( thread_IDs[i], NULL );
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
