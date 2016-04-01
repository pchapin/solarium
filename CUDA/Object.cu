/*! \file    Object.c
    \brief   Implementation of object data arrays.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "global.h"
#include "Initialize.h"
#include "CUDAVector3.h"

Object         *object_array;
ObjectDynamics *current_dynamics;
ObjectDynamics *next_dynamics;


__global__ void do_calculations(
    Object *object_array, ObjectDynamics *current_dynamics, ObjectDynamics *next_dynamics )
{
    int object_i = blockIdx.x*blockDim.x + threadIdx.x;

    // For each object...
    if( object_i < OBJECT_COUNT ) {
        Vector3 total_force = { 0.0F, 0.0F, 0.0F };

        // Consider interactions with all other objects...
        for( int object_j = 0; object_j < OBJECT_COUNT; ++object_j ) {
            if( object_i == object_j ) continue;

            Vector3 displacement = cuda_v3_subtract(
                current_dynamics[object_j].position, current_dynamics[object_i].position );
            float distance_squared = cuda_magnitude_squared( displacement );
            float distance = sqrt( distance_squared );
            float t1 = object_array[object_i].mass / distance;
            float t2 = object_array[object_j].mass / distance;
            //float force_magnitude =
            //    ( G * object_array[object_i].mass * object_array[object_j].mass ) / distance_squared;
            float force_magnitude = ( G * t1 ) * t2;
            Vector3 force = cuda_v3_multiply( (force_magnitude / distance ), displacement );
            total_force = cuda_v3_add( total_force, force );
        }

        // Total force on object_i is now known. Compute acceleration, velocity and position.
        Vector3 acceleration =
            cuda_v3_divide( total_force, object_array[object_i].mass );

        Vector3 delta_v =
            cuda_v3_multiply( TIME_STEP, acceleration );

        Vector3 delta_position =
            cuda_v3_multiply( TIME_STEP, current_dynamics[object_i].velocity );

        // Update the dynamics arrays.
        next_dynamics[object_i].velocity =
            cuda_v3_add( current_dynamics[object_i].velocity, delta_v );

        next_dynamics[object_i].position =
            cuda_v3_add( current_dynamics[object_i].position, delta_position );
    }
}


void cuda_time_step(
    Object         *dev_object_array,
    ObjectDynamics *dev_current_dynamics,
    ObjectDynamics *dev_next_dynamics )
{
    int block_size = 256;  // Number of threads per block.
    int block_count =
        (OBJECT_COUNT % block_size == 0) ? OBJECT_COUNT/block_size
                                         : OBJECT_COUNT/block_size + 1;

    // Strictly speaking we don't need to copy the current_dynamics array back and forth.
    // The computed values could be left on the device during the whole computation and copied
    // back only at the end. However, experiments show the overhead of this copying is small
    // compared to the time required to do the computation above.

    cudaMemcpy( dev_current_dynamics,
                current_dynamics,
                OBJECT_COUNT*sizeof(ObjectDynamics),
                cudaMemcpyHostToDevice );

    do_calculations<<<block_count, block_size>>>(
        dev_object_array, dev_current_dynamics, dev_next_dynamics );

    cudaMemcpy( current_dynamics,
                dev_next_dynamics,
                OBJECT_COUNT*sizeof(ObjectDynamics),
                cudaMemcpyDeviceToHost );
}


void dump_dynamics( )
{
    for( int object_i = 0; object_i < OBJECT_COUNT; ++object_i ) {
        printf( "%4d: x = %10.3E, y = %10.3E, z = %10.3E\n", object_i,
                current_dynamics[object_i].position.x/AU,
                current_dynamics[object_i].position.y/AU,
                current_dynamics[object_i].position.z/AU );
    }
}
