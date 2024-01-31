/*!
 * \file    global.h
 * \brief   Definition of constants and of object data types and arrays.
 * \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#ifndef GLOBAL_H
#define GLOBAL_H

#include "Vector3.h"

// We need this custom function for macOS.
#if defined(__APPLE__)
int get_macOS_nprocs( void );
#endif

// These constants can change the behavior of all programs that share this header.
#define OBJECT_COUNT     10000
#define AU               1.49597870700E+11  // Meters per astronomical unit.
#define AVERAGE_VELOCITY 2.9785E+3   // Meters per second (used during random initialization).
#define G                6.673E-11   // Gravitational constant.
#define TIME_STEP        3.6E+03     // Seconds in a time step (one hour).
// The AVERAGE_VELOCITY above is 1/10 the magnitude of the velocity of Earth in its orbit.

//! Structure that represents the position and velocity of a particular object.
/*!
 * This structure encapsulates the dynamic properties of an object.
 */
typedef struct {
    Vector3 position;
    Vector3 velocity;
} ObjectDynamics;

//! Structure that represents an individual object.
/*!
 * This class defines the object's unchanging mass.
 */
typedef struct {
    double mass;
} Object;

// Three arrays are used to hold object information. These arrays are indexed by an integer
// object ID. The current_dynamics array contains the current position and velocity of all the
// objects. The next_dynamics array contains the estimated position and velocity of all the
// objects after the current time step. The object with object ID of zero is the sun.
//
extern Object         *object_array;
extern ObjectDynamics *current_dynamics;
extern ObjectDynamics *next_dynamics;

//! Compute the next dynamics from the current dynamics.
/*!
 * This function takes one step of simulated time using the value of TIME_STEP as the step size.
 * It updates the next dynamics array and swaps the array so that when it returns the updated
 * state of the system is in current_dynamics.
 */
void time_step( );


//! Print the current dynamics.
void dump_dynamics( );

#endif
