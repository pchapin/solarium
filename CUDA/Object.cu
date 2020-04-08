/*! \file    Object.c
 *  \brief   Implementation of object data arrays.
 *  \author  Peter C. Chapin <pchapin@vtc.edu>
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

#define BLOCK_SIZE 256

__global__ void do_calculations(
    Object *object_array, ObjectDynamics *current_dynamics, ObjectDynamics *next_dynamics )
{
    int object_i = blockIdx.x * blockDim.x + threadIdx.x;

    // For each object...
    if( object_i < OBJECT_COUNT ) {
        Vector3 total_force = { 0.0F, 0.0F, 0.0F };

        const Vector3 pos_i = current_dynamics[object_i].position;
        const float mass_i = object_array[object_i].mass;

        // Shared memory is shared between all the threads of a block.
        __shared__ Vector3 shared_position[BLOCK_SIZE];
        __shared__ float shared_mass[BLOCK_SIZE];

        int blocks = (OBJECT_COUNT + BLOCK_SIZE - 1) / BLOCK_SIZE;

        // We consider a block's worth of "other" objects at a time.
        for (unsigned block_i = 0; block_i < blocks-1; block_i++) {

            // Populate the shared information (essentially, cache it).
            // Since everythread in the block is doing this, the entire shared arrays are filled.
            shared_position[threadIdx.x] = current_dynamics[block_i * blockDim.x + threadIdx.x].position;
            shared_mass[threadIdx.x] = object_array[block_i * blockDim.x + threadIdx.x].mass;

            // Wait for all the threads in the block to finish populating the shared arrays.
            __syncthreads();

            // Consider interactions with all other objects...
            // Here object_i is an overall object index, but object_j is a per-block object index.
            for (int object_j = 0; object_j < BLOCK_SIZE; ++object_j) {
                if( object_i == block_i * blockDim.x + object_j ) continue;

                Vector3 displacement = cuda_v3_subtract( shared_position[object_j], pos_i );
                const float distance_squared = cuda_magnitude_squared( displacement );
                const float distance = sqrt( distance_squared );
                const float t1 = mass_i / distance;
                const float t2 = shared_mass[object_j] / distance;
                //float force_magnitude =
                //    ( G * object_array[object_i].mass * object_array[object_j].mass ) / distance_squared;
                const float force_magnitude = ( G * t1 ) * t2;
                const Vector3 force = cuda_v3_multiply( (force_magnitude / distance ), displacement );
                total_force = cuda_v3_add( total_force, force );
            }

            
            // Wait for all threads in the block to finish computations before loading the next block.
            __syncthreads();
        }

        // Now we must deal with the last partial block...

        if ((blocks-1) * blockDim.x + threadIdx.x < OBJECT_COUNT) {
            shared_position[threadIdx.x] =
                current_dynamics[(blocks-1) * blockDim.x + threadIdx.x].position;
            shared_mass[threadIdx.x] =
                object_array[(blocks-1) * blockDim.x + threadIdx.x].mass;
        }
        __syncthreads();
        for (int object_j = 0; object_j < OBJECT_COUNT - BLOCK_SIZE * (blocks-1); ++object_j) {
            if( object_i == (blocks-1) * blockDim.x + object_j ) continue;

            Vector3 displacement = cuda_v3_subtract( shared_position[object_j], pos_i );
            float distance_squared = cuda_magnitude_squared( displacement );
            float distance = sqrt( distance_squared );
            float t1 = mass_i / distance;
            float t2 = shared_mass[object_j] / distance;
            //float force_magnitude =
            //    ( G * object_array[object_i].mass * object_array[object_j].mass ) / distance_squared;
            float force_magnitude = ( G * t1 ) * t2;
            Vector3 force = cuda_v3_multiply( (force_magnitude / distance ), displacement );
            total_force = cuda_v3_add( total_force, force );
        }

        // Total force on object_i is now known. Compute acceleration, velocity and position.
        Vector3 acceleration   = cuda_v3_divide( total_force, mass_i );
        Vector3 delta_v        = cuda_v3_multiply( TIME_STEP, acceleration );
        Vector3 delta_position = cuda_v3_multiply( TIME_STEP, current_dynamics[object_i].velocity );

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
    int block_count =
        (OBJECT_COUNT % BLOCK_SIZE == 0) ? OBJECT_COUNT/BLOCK_SIZE
                                         : OBJECT_COUNT/BLOCK_SIZE + 1;

    // Strictly speaking we don't need to copy the current_dynamics array back and forth. The
    // computed values could be left on the device during the whole computation and copied back
    // only at the end. [But be careful: you still need to swap the dynamics arrays on the
    // device between time steps]. However, experiments show the overhead of this copying is
    // small compared to the time required to do the computation above.

    cudaMemcpy( dev_current_dynamics,
                current_dynamics,
                OBJECT_COUNT * sizeof(ObjectDynamics),
                cudaMemcpyHostToDevice );

    size_t size_shared = BLOCK_SIZE * (sizeof(next_dynamics[0].position) + sizeof(object_array[0].mass));
    do_calculations<<<block_count, BLOCK_SIZE, size_shared>>>(
        dev_object_array, dev_current_dynamics, dev_next_dynamics );

    // Notice that we copy the next_dynamics on the device to the current_dynamics on the host.
    // This means we don't have to worry about swapping the arrays explicitly.
    cudaMemcpy( current_dynamics,
                dev_next_dynamics,
                OBJECT_COUNT * sizeof(ObjectDynamics),
                cudaMemcpyDeviceToHost );
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
