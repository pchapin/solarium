/*!
 * \file    Object.h
 * \brief   Definition of a object data arrays.
 * \author  Peter C. Chapin <pchapin@vtc.edu>
 */

#ifndef OBJECT_H
#define OBJECT_H

#include "Vector3.h"

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

// Three parallel arrays are used to hold object information. These arrays are indexed by an
// integer object ID. The current_dynamics array contains the current position and velocity of
// all the objects. The next_dynamics array contains the estimated position and velocity of all
// the objects after the current time step. The object with object ID of zero is the sun.
//
extern Object         *object_array;
extern ObjectDynamics *current_dynamics;
extern ObjectDynamics *next_dynamics;

//! Allocate and initialize the global arrays mentioned above.
/*!
 * This function uses external configuration information to create the necessary arrays.
 * [TODO: Note the possible errors here.]
 */
void initialize_object_arrays( );

//! Compute the next dynamics from the current dynamics.
/*!
 * This function takes one step of simulated time.
 *
 * \param start_index The object ID of the first object managed by this thread.
 * \param end_index The object ID just past the last object managed by this thread.
 */
void time_step( int start_index, int end_index );

//! Print the current dynamics.
void dump_dynamics( );

#endif
