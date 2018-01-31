/*! \file    Object.c
    \brief   Implementation of object data arrays.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#ifdef __GLIBC__
#include <sys/sysinfo.h>
#endif

#include "global.h"
#include "Initialize.h"

Object         *object_array;
ObjectDynamics *current_dynamics;
ObjectDynamics *next_dynamics;

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
    #ifdef __GLIBC__
    int processor_count = get_nprocs( );
    #else
    int processor_count = pthread_num_processors_np( );
    #endif

    struct Work_Unit *chunks =
        (struct Work_Unit *)malloc( processor_count * sizeof(struct Work_Unit) );
    pthread_t *threads =
        (pthread_t *)malloc( processor_count * sizeof(pthread_t) );
    
    compute_next_dynamics...
        
    // Swap the dynamics arrays.
    ObjectDynamics *temp = current_dynamics;
    current_dynamics     = next_dynamics;
    next_dynamics        = temp;

    free(chunks);
    free(threads);
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
