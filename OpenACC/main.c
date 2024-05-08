/*! \file    main.c
 *  \brief   Main program of the OpenACC solar system simulator.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
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

    initialize_object_arrays( );
    Timer_initialize( &stopwatch );
    printf( "START position\n" );
    dump_dynamics( );
    Timer_start( &stopwatch );
    while( 1 ) {
        time_step( );
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
    Timer_stop( &stopwatch );
    printf( "\nEND position\n" );
    dump_dynamics( );
    printf( "Time elapsed = %ld milliseconds\n", Timer_time( &stopwatch ) );

    return return_code;
}
