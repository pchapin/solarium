/*! \file    Initialize.c
 *  \brief   Implementation of initialization functions.
 *  \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
 */

#include <stdlib.h>
#include "global.h"
#include "Initialize.h"

//! Return a random coordinate inside a 100.0 AU cube about the origin.
static double random_position_coordinate( )
{
    double fraction = (double)( rand( ) ) / RAND_MAX;
    return ( ( fraction * 100.0 ) - 50.0 ) * AU;
}


//! Return a random velocity that is a fraction of AVERAGE_VELOCITY.
static double random_velocity_component( )
{
    double fraction = (double)( rand( ) ) / RAND_MAX;
    int    sign     = rand( ) % 2;

    if( sign == 0 ) return fraction * AVERAGE_VELOCITY;
    return -fraction * AVERAGE_VELOCITY;
}


// For now, just use some arbitrary values to show the nature of the computation.
void initialize_object_arrays( )
{
    ObjectDynamics *A;
    ObjectDynamics *B;
    Vector3 temp;

    // TODO: Check for NULL pointer returns from the memory allocation operations.
    object_array = (Object *)malloc(OBJECT_COUNT * sizeof(Object));

    // The sun has ID 0.
    //object_array[0].mass = 1.98892E+30;  // Mass of the sun.
    object_array[0].mass = 5.9722E+24;   // Mass of the Earth.

    // Initialize the object array to OBJECT_COUNT Earth sized objects.
    for( int i = 1; i < OBJECT_COUNT; ++i ) {
        object_array[i].mass = 5.9722E+24;   // Mass of the Earth.
    }

    A = (ObjectDynamics *)malloc(OBJECT_COUNT * sizeof(ObjectDynamics));
    B = (ObjectDynamics *)malloc(OBJECT_COUNT * sizeof(ObjectDynamics));

    // The sun is at the origin with zero velocity.
    temp.x = temp.y = temp.z = 0.0;
    A[0].position = temp;
    A[0].velocity = temp;

    // TODO: Use a more "random" seed?
    srand( 0 );
    for( int i = 1; i < OBJECT_COUNT; ++i ) {

        // Randomly position object i.
    	temp.x = random_position_coordinate( );
    	temp.y = random_position_coordinate( );
    	temp.z = random_position_coordinate( );
    	A[i].position = temp;

        // Select a random velocity for object i.
    	temp.x = random_velocity_component( );
    	temp.y = random_velocity_component( );
    	temp.z = random_velocity_component( );
    	A[i].velocity = temp;

    	// No need to initialize the next dynamics. It will be computed fresh immediately.
    }
    current_dynamics = A;
    next_dynamics = B;
}
