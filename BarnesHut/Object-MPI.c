/*! \file    Object.c
 *  \brief   Implementation of object data arrays.
 *  \author  Peter C. Chapin <pchapin@vtc.edu>
 */

#include <stdio.h>
#include <math.h>
#include <mpi.h>
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
    #pragma omp parallel for
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


// WARNING: This is horrible!
// Break the abstraction of Vector3 so we can explain to MPI the layout of this structure.
struct HorrifyingHack {
   double position[3];
   double velocity[3];
};


// Explain to MPI about the StarDynamics structure.
void build_MPI_dynamics_type( MPI_Datatype *type )
{
    int          block_lengths[2];
    MPI_Aint     offsets[2];
    MPI_Datatype types[2];

    block_lengths[0] = 3;
    block_lengths[1] = 3;
    offsets[0] = offsetof( struct HorrifyingHack, position );
    offsets[1] = offsetof( struct HorrifyingHack, velocity );
    types[0]   = MPI_DOUBLE;
    types[1]   = MPI_DOUBLE;

    MPI_Type_struct( 2, block_lengths, offsets, types, type );
}


void time_step( )
{
    Octree spacial_tree;
    int number_of_nodes;
    int my_rank;
    MPI_Datatype dynamics_type;

    MPI_Comm_size( MPI_COMM_WORLD, &number_of_nodes );
    MPI_Comm_rank( MPI_COMM_WORLD, &my_rank );
    build_MPI_dynamics_type( &dynamics_type );
    MPI_Type_commit( &dynamics_type );

    MPI_Bcast( current_dynamics, OBJECT_COUNT, dynamics_type, 0, MPI_COMM_WORLD );

    // How many objects is each MPI node handling?
    int objects_per_node = OBJECT_COUNT / number_of_nodes;
    int my_object_count  = objects_per_node;
    if( my_rank == number_of_nodes - 1 ) {
        // NOTE: If any of the nodes have a different my_object_count, MPI_Gather won't work!
        // Thus it is necessary for OBJECT_COUNT % number_of_nodes == 0.
        my_object_count += OBJECT_COUNT % number_of_nodes;
    }
    int start_index = my_rank * objects_per_node;
    int end_index = start_index + my_object_count;

    // Every node builds its own Octree.
    Octree_init( &spacial_tree, &overall_region );
    for( int i = 0; i < OBJECT_COUNT; ++i ) {
        Octree_insert( &spacial_tree, current_dynamics[i].position, object_array[i].mass );
    }
    Octree_refresh_interior( &spacial_tree );
    CPU_work_unit( &spacial_tree, start_index, end_index );
    Octree_destroy( &spacial_tree );

    // Bring together the results into the master's current_dynamics array.
    ObjectDynamics *src = &next_dynamics[start_index];
    ObjectDynamics *tgt = current_dynamics;

    // Many applications can use something like this but in this case it would be wrong.
    // src = (my_rank == 0) ? MPI_IN_PLACE : src;
    MPI_Gather(
        src, my_object_count, dynamics_type,
        tgt, my_object_count, dynamics_type, 0, MPI_COMM_WORLD );
    MPI_Type_free( &dynamics_type );

    // Don't need to swap dynamics arrays because MPI_Gather took care of that.
    //
    // // Swap the dynamics arrays.
    // ObjectDynamics *temp = current_dynamics;
    // current_dynamics     = next_dynamics;
    // next_dynamics        = temp;
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

