/*! \file    Object.c
 *  \brief   Implementation of object data arrays.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "global.h"

Object         *object_array;
ObjectDynamics *current_dynamics;
ObjectDynamics *next_dynamics;


void time_step( )
{
    int object_i;

    // For each object...
    #pragma acc enter data copyin(current_dynamics[0:OBJECT_COUNT], object_array[0:OBJECT_COUNT])
    #pragma acc parallel loop
    for( object_i = 0; object_i < OBJECT_COUNT; ++object_i ) {
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
        Vector3 acceleration   = v3_divide( total_force, object_array[object_i].mass);
        Vector3 delta_v        = v3_multiply( TIME_STEP, acceleration );
        Vector3 delta_position = v3_multiply( TIME_STEP, current_dynamics[object_i].velocity );

        next_dynamics[object_i].velocity =
            v3_add( current_dynamics[object_i].velocity, delta_v );

        next_dynamics[object_i].position =
            v3_add( current_dynamics[object_i].position, delta_position );
    }
    #pragma acc exit data copyout(next_dynamics[0:OBJECT_COUNT])

    // Swap the dynamics arrays.
    ObjectDynamics *temp = current_dynamics;
    current_dynamics     = next_dynamics;
    next_dynamics        = temp;
}


void dump_dynamics( )
{
    for( int object_i = 0; object_i < OBJECT_COUNT; ++object_i ) {
        printf( "%4d: x = %11.3E, y = %11.3E, z = %11.3E\n", object_i,
            current_dynamics[object_i].position.x / AU,
            current_dynamics[object_i].position.y / AU,
            current_dynamics[object_i].position.z / AU );
    }
}
