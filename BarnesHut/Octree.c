/*! \file Octree.c
 *  \brief Implementation of octrees and their operations.
 *  \author Peter C. Chapin <pchapin@vtc.edu>
 *  \date October 22, 2012
 */

#include <math.h>
#include <stdlib.h>
#include "Octree.h"

#define TRUE  1
#define FALSE 0
#define G     6.673E-11   // Gravitational constant in MKS units.

static void subtree_destroy( /*@null@*/ /*@only@*/ struct OctreeNode *subtree )
{
    int i;

    if( subtree == NULL ) return;
    for( i = 0; i < 8; ++i ) {
        subtree_destroy( subtree->octants[i] );
    }
    free( subtree );
}

//  x > 0, y > 0, z > 0  ==> 0
//  x > 0, y > 0, z < 0  ==> 1
//  x > 0, y < 0, z > 0  ==> 2
//  x > 0, y < 0, z < 0  ==> 3
//  x < 0, y > 0, z > 0  ==> 4
//  x < 0, y > 0, z < 0  ==> 5
//  x < 0, y < 0, z > 0  ==> 6
//  x < 0, y < 0, z < 0  ==> 7
//
static int get_octant( struct OctreeNode *overall, struct OctreeNode *particular )
{
   double center_x = (overall->region.x_interval.max + overall->region.x_interval.min) / 2.0;
   double center_y = (overall->region.y_interval.max + overall->region.y_interval.min) / 2.0;
   double center_z = (overall->region.z_interval.max + overall->region.z_interval.min) / 2.0;

   double x = particular->center_of_mass.x - center_x;
   double y = particular->center_of_mass.y - center_y;
   double z = particular->center_of_mass.z - center_z;

   if( x >= 0.0 && y >= 0.0 && z >= 0.0 ) return 0;
   if( x >= 0.0 && y >= 0.0 && z <  0.0 ) return 1;
   if( x >= 0.0 && y <  0.0 && z >= 0.0 ) return 2;
   if( x >= 0.0 && y <  0.0 && z <  0.0 ) return 3;
   if( x <  0.0 && y >= 0.0 && z >= 0.0 ) return 4;
   if( x <  0.0 && y >= 0.0 && z <  0.0 ) return 5;
   if( x <  0.0 && y <  0.0 && z >= 0.0 ) return 6;
   // if( x <  0.0 && y <  0.0 && z <  0.0 )
   return 7;
}


static Box get_region( struct OctreeNode *overall, int octant_index )
{
    double center_x = (overall->region.x_interval.max + overall->region.x_interval.min) / 2.0;
    double center_y = (overall->region.y_interval.max + overall->region.y_interval.min) / 2.0;
    double center_z = (overall->region.z_interval.max + overall->region.z_interval.min) / 2.0;
    Box    subregion = overall->region;

    switch( octant_index ) {
    case 0:
        subregion.x_interval.min = center_x;
        subregion.y_interval.min = center_y;
        subregion.z_interval.min = center_z;
        break;
    case 1:
        subregion.x_interval.min = center_x;
        subregion.y_interval.min = center_y;
        subregion.z_interval.max = center_z;
        break;
    case 2:
        subregion.x_interval.min = center_x;
        subregion.y_interval.max = center_y;
        subregion.z_interval.min = center_z;
        break;
    case 3:
        subregion.x_interval.min = center_x;
        subregion.y_interval.max = center_y;
        subregion.z_interval.max = center_z;
        break;
    case 4:
        subregion.x_interval.max = center_x;
        subregion.y_interval.min = center_y;
        subregion.z_interval.min = center_z;
        break;
    case 5:
        subregion.x_interval.max = center_x;
        subregion.y_interval.min = center_y;
        subregion.z_interval.max = center_z;
        break;
    case 6:
        subregion.x_interval.max = center_x;
        subregion.y_interval.max = center_y;
        subregion.z_interval.min = center_z;
        break;
    case 7:
        subregion.x_interval.max = center_x;
        subregion.y_interval.max = center_y;
        subregion.z_interval.max = center_z;
        break;
    }
    return subregion;
}


static void subtree_insert(
    /*@dependent@*/ struct OctreeNode *current,
    /*@only@*/      struct OctreeNode *new_node )
{
    int octant_index;

    // If 'current' is not a leaf node...
    if( !current->is_leaf ) {
        octant_index = get_octant( current, new_node );
        if( current->octants[octant_index] == NULL ) {
            new_node->region = get_region( current, octant_index );
            current->octants[octant_index] = new_node;
        }
        else {
            subtree_insert( current->octants[octant_index], new_node );
        }
    }
    else {
        // Subdivide the curent node.
        struct OctreeNode *subnode = (struct OctreeNode *)malloc( sizeof(struct OctreeNode) );
        // TODO: What if subnode == NULL?
        *subnode = *current;
        current->is_leaf = FALSE;
        subtree_insert( current, subnode );    // Insert original item into appropriate octant.
        subtree_insert( current, new_node );   // Retry at the current node.
    }
}


static void subtree_refresh( struct OctreeNode *node )
{
    double x = 0.0, y = 0.0, z = 0.0;

    if( node->is_leaf ) return;

    node->total_mass = 0.0;
    for( int i = 0; i < 8; ++i ) {
        if( node->octants[i] != NULL ) {
            subtree_refresh( node->octants[i] );
            node->total_mass += node->octants[i]->total_mass;
            x += node->octants[i]->center_of_mass.x * node->octants[i]->total_mass;
            y += node->octants[i]->center_of_mass.y * node->octants[i]->total_mass;
            z += node->octants[i]->center_of_mass.z * node->octants[i]->total_mass;
        }
    }
    node->center_of_mass.x = x / node->total_mass;
    node->center_of_mass.y = y / node->total_mass;
    node->center_of_mass.z = z / node->total_mass;
}


static Vector3 subtree_force( struct OctreeNode *node, Vector3 position, double mass )
{
    static const double theta = 0.5;
    Vector3 force = { 0.0, 0.0, 0.0 };

    // Ignore this node if it's the node containing the object under consideration.
    if( node->is_leaf &&
        node->center_of_mass.x == position.x &&
        node->center_of_mass.y == position.y &&
        node->center_of_mass.z == position.z) return force;

    double d  = sqrt( magnitude_squared( v3_subtract(node->center_of_mass, position) ) );
    double sx = node->region.x_interval.max - node->region.x_interval.min;
    double sy = node->region.y_interval.max - node->region.y_interval.min;
    double sz = node->region.z_interval.max - node->region.z_interval.min;
    double s;
    // Find the max(sx, sy, sz)
    if( sx > sy ) {
        s = (sx > sz) ? sx : sz;
    }
    else {
        s = (sy > sz) ? sy : sz;
    }

    // If this region is a leaf or "far enough" away from the object, do the direct computation.
    if( node->is_leaf || s/d < theta) {
        Vector3 displacement = v3_subtract( node->center_of_mass, position );
        double distance_squared = magnitude_squared( displacement );
        double distance = sqrt( distance_squared );
        double force_magnitude = ( G * mass * node->total_mass ) / distance_squared;
        force = v3_multiply( (force_magnitude / distance ), displacement );
    }
    // Otherwise recursively examine the octants.
    else {
        for( int i = 0; i < 8; ++i ) {
            if( node->octants[i] != NULL ) {
                force = v3_add( force, subtree_force( node->octants[i], position, mass ) );
            }
        }
    }
    return force;
}


void Octree_init( Octree *tree, Box *overall_region )
{
    // Provide default initial values.
    tree->root           =  NULL;
    tree->overall_region = *overall_region;
}


int Octree_insert( Octree *tree, Vector3 position, double mass )
{
    int i;
    struct OctreeNode *new_node;

    // Set up the new node.
    new_node = (struct OctreeNode *)malloc( sizeof( struct OctreeNode ) );
    if( new_node == NULL ) return -1;
    for( i = 0; i < 8; ++i ) {
        new_node->octants[i] = NULL;
    }
    new_node->is_leaf = TRUE;
    new_node->center_of_mass = position;
    new_node->total_mass = mass;
    // new_node->region will be defined later.

    // Add it to the tree.
    if( tree->root == NULL ) {
        new_node->region = tree->overall_region;
        tree->root = new_node;
    }
    else {
        subtree_insert( tree->root, new_node );
    }
    return 0;
}


void Octree_refresh_interior( Octree *tree )
{
    if( tree->root != NULL ) {
        subtree_refresh( tree->root );
    }
}


Vector3 Octree_force( Octree *tree, Vector3 position, double mass )
{
    Vector3 force = { 0.0, 0.0, 0.0 };

    if( tree->root != NULL ) {
        force = subtree_force( tree->root, position, mass );
    }
    return force;
}


void Octree_destroy( Octree *tree )
{
    // Deallocate all the tree nodes.
    subtree_destroy( tree->root );

    // Put the left over tree object into a well defined state.
    tree->root  = NULL;
}
