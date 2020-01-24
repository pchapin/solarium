/*! \file    Vector3.h
    \brief   Handling of three dimensional vectors.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#ifndef VECTOR3_H
#define VECTOR3_H

//! Structure that represents a three dimensional vector.
/*!
 *  This structure simplifies vector computations by encapsulating the handling of the vector's
 *  components.
 */
typedef struct {
	double x;
	double y;
	double z;
} Vector3;

#ifdef __cplusplus
extern "C" {
#endif

Vector3 v3_add( Vector3 a, Vector3 b );
Vector3 v3_subtract( Vector3 a, Vector3 b );
Vector3 v3_multiply( double scale_factor, Vector3 a );
Vector3 v3_divide( Vector3 a, double scale_factor );

double magnitude_squared( Vector3 a );

#ifdef __cplusplus
}
#endif

#endif
