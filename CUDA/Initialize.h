/*! \file    Initialize.h
 *  \brief   Declarations initialization functions.
 *  \author  Peter Chapin <peter.chapin@vermontstate.edu>
*/

#ifndef INITIALIZE_H
#define INITIALIZE_H

#ifdef __cplusplus
extern "C" {
#endif

//! Allocate and initialize the global arrays mentioned above.
/*!
 * This function uses external configuration information to create the necessary arrays.
 * TODO: Note the possible errors here.
 */
void initialize_object_arrays( );

#ifdef __cplusplus
}
#endif

#endif // INITIALIZE_H
