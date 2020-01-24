/*! \file    Vector3.c
    \brief   Implementation of the handling of three dimensional vectors.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#include "Vector3.h"

Vector3 v3_add( Vector3 a, Vector3 b )
{
	Vector3 result;
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	result.z = a.z + b.z;
	return result;
}

Vector3 v3_subtract( Vector3 a, Vector3 b )
{
	Vector3 result;
	result.x = a.x - b.x;
	result.y = a.y - b.y;
	result.z = a.z - b.z;
	return result;
}

Vector3 v3_multiply( double scale_factor, Vector3 a )
{
	Vector3 result = a;
	result.x *= scale_factor;
	result.y *= scale_factor;
	result.z *= scale_factor;
	return result;
}

Vector3 v3_divide( Vector3 a, double scale_factor )
{
	Vector3 result = a;
	result.x /= scale_factor;
	result.y /= scale_factor;
	result.z /= scale_factor;
	return result;
}

double magnitude_squared( Vector3 a )
{
	return (a.x * a.x) + (a.y * a.y) + (a.z * a.z);
}
