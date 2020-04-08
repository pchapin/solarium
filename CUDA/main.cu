/*! \file    main.c
 *  \brief   Main program of the CUDA solar system simulator.
 *  \author  Peter C. Chapin <pchapin@vtc.edu>
 *
 * LICENSE
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of
 * the GNU General Public License as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if
 * not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA
 */

#include <stdio.h>
#include <stdlib.h>

#include "global.h"
#include "Initialize.h"
#include "Timer.h"

#define STEPS_PER_YEAR 8766  // Number of hours in a year.

int main( int argc, char **argv )
{
    Timer stopwatch;
    long long total_steps = 0;
    int total_years       = 0;
    int return_code       = EXIT_SUCCESS;

    Object         *dev_object_array;       // Pointer to object array on GPU.
    ObjectDynamics *dev_current_dynamics;   // Pointer to current dynamics array on GPU.
    ObjectDynamics *dev_next_dynamics;      // Pointer to next dynamics array on GPU.

    initialize_object_arrays( );
    printf( "START position\n" );
    dump_dynamics( );
    Timer_initialize( &stopwatch );
    Timer_start( &stopwatch );

    // Allocate memory on GPU.
    // TODO: Add error checking!
    cudaMalloc( (void**)&dev_object_array, OBJECT_COUNT*sizeof(Object) );
    cudaMalloc( (void**)&dev_current_dynamics, OBJECT_COUNT*sizeof(ObjectDynamics) );
    cudaMalloc( (void**)&dev_next_dynamics, OBJECT_COUNT*sizeof(ObjectDynamics) );

    // Copy the object array to the GPU. This never changes so this only needs to be done once.
    cudaMemcpy( dev_object_array,
                object_array,
                OBJECT_COUNT*sizeof(Object),
                cudaMemcpyHostToDevice );

    // Maybe copy the dynamics arrays once and leave them on the device...? See note in Object.cu.
    //
    //cudaMemcpy( dev_current_dynamics,
    //            current_dynamics,
    //            OBJECT_COUNT*sizeof(ObjectDynamics),
    //            cudaMemcpyHostToDevice );

    while (1) {
        cuda_time_step( dev_object_array, dev_current_dynamics, dev_next_dynamics );
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

            // For now, stop the simulation after 1 year.
            if( total_years == 1 ) break;
        }
    }

    //cudaMemcpy( current_dynamics,
    //            dev_next_dynamics,
    //            OBJECT_COUNT*sizeof(ObjectDynamics),
    //            cudaMemcpyDeviceToHost );

    // Deallocate memory on GPU.
    cudaFree( dev_object_array );
    cudaFree( dev_current_dynamics );
    cudaFree( dev_next_dynamics );

    Timer_stop( &stopwatch );
    printf( "\nEND position\n" );
    dump_dynamics( );
    printf( "Time elapsed = %ld milliseconds\n", Timer_time( &stopwatch ) );

    return return_code;
}
