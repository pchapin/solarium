/*! \file    CUDAVector3.h
 *  \brief   Handling of three dimensional vectors on the GPU.
 *  \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
 */

#ifndef CUDAVECTOR3_H
#define CUDAVECTOR3_H

//! Structure that represents a three dimensional vector.
/*!
 *  This structure simplifies vector computations by encapsulating the handling of the vector's
 *  components.
 */
typedef struct {
    float x;
    float y;
    float z;
} Vector3;


__device__ inline Vector3 cuda_v3_add( Vector3 a, Vector3 b )
{
    Vector3 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    return result;
}

__device__ inline Vector3 cuda_v3_subtract( Vector3 a, Vector3 b )
{
    Vector3 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    return result;
}

__device__ inline Vector3 cuda_v3_multiply( float scale_factor, Vector3 a )
{
    Vector3 result = a;
    result.x *= scale_factor;
    result.y *= scale_factor;
    result.z *= scale_factor;
    return result;
}

__device__ inline Vector3 cuda_v3_divide( Vector3 a, float scale_factor )
{
    Vector3 result = a;
    result.x /= scale_factor;
    result.y /= scale_factor;
    result.z /= scale_factor;
    return result;
}

__device__ inline float cuda_magnitude_squared( Vector3 a )
{
    return (a.x * a.x) + (a.y * a.y) + (a.z * a.z);
}


#endif
