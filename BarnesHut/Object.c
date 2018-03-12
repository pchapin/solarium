/*! \file    Object.c
 *  \brief   Implementation of object data arrays.
 *  \author  Peter C. Chapin <pchapin@vtc.edu>
 */

#include <stdio.h>
#include <math.h>
#include "global.h"
#include "Octree.h"

Object         *object_array;
ObjectDynamics *current_dynamics;
ObjectDynamics *next_dynamics;

Box overall_region = {
    .x_interval = { -100.0 * AU, 100.0 * AU },
    .y_interval = { -100.0 * AU, 100.0 * AU },
    .z_interval = { -100.0 * AU, 100.0 * AU }
};


void CPU_work_unit( Octree *spacial_tree, int start_index, int stop_index )
{
    // For each object...
    for( int object_i = start_index; object_i < stop_index; ++object_i ) {
        Vector3 total_force =
            Octree_force( spacial_tree,
                          current_dynamics[object_i].position,
                          object_array[object_i].mass );

        // Total force on object_i is now known. Compute acceleration, velocity and position.
        Vector3 acceleration   = v3_divide( total_force, object_array[object_i].mass );
        Vector3 delta_v        = v3_multiply( TIME_STEP, acceleration );
        Vector3 delta_position = v3_multiply( TIME_STEP, current_dynamics[object_i].velocity );

        next_dynamics[object_i].velocity =
            v3_add( current_dynamics[object_i].velocity, delta_v );

        next_dynamics[object_i].position =
            v3_add( current_dynamics[object_i].position, delta_position );
    }
}


void time_step( )
{
    Octree spacial_tree;
    int number_of_nodes;

    // Build the Octree.
    Octree_init( &spacial_tree, &overall_region );
    for( int i = 0; i < OBJECT_COUNT; ++i ) {
        Octree_insert( &spacial_tree, current_dynamics[i].position, object_array[i].mass );
    }
    Octree_refresh_interior( &spacial_tree );
    CPU_work_unit( &spacial_tree, 0, OBJECT_COUNT );
    Octree_destroy( &spacial_tree );

    // Swap the dynamics arrays.
    ObjectDynamics *temp = current_dynamics;
    current_dynamics     = next_dynamics;
    next_dynamics        = temp;
}


void dump_dynamics( )
{
    for( int object_i = 0; object_i < OBJECT_COUNT; ++object_i ) {
        printf( "%4d: x = %11.3E, y = %11.3E, z = %11.3E\n", object_i,
            current_dynamics[object_i].position.x / AU,
            current_dynamics[object_i].position.y / AU,
            current_dynamics[object_i].position.z / AU );
    }
}

