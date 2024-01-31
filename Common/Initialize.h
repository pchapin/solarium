/*! \file    Initialize.h
 *  \brief   Declarations initialization functions.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#ifndef INITIALIZE_H
#define INITIALIZE_H

#ifdef __cplusplus
extern "C" {
#endif

//! Allocate and initialize the global arrays mentioned above.
/*!
 * This function (eventually) uses external configuration information to create the necessary
 * arrays. At the moment it does a silly initialization of the arrays by just populating them
 * with OBJECT_COUNT Earth-sized objects positioned randomly in a cube with a side length of 100
 * AU. The objects are given random initial velocities. [TODO: Note the possible errors here.]
 */
void initialize_object_arrays( );

#ifdef __cplusplus
}
#endif

#endif // INITIALIZE_H
