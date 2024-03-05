/*! \file Octree.h
 *  \brief Declarations of Octree and its operations.
 *  \author Peter Chapin <spicacality@kelseymountain.org>
 */

#ifndef OCTREE_H
#define OCTREE_H

#include "Interval.h"
#include "Vector3.h"

struct OctreeNode {
    struct OctreeNode *octants[8];
    int     is_leaf;
    Box     region;
    Vector3 center_of_mass;
    double  total_mass;
};

typedef struct {
    struct OctreeNode *root;
    Box    overall_region;
} Octree;

void    Octree_init( Octree *tree, Box *overall_region );
int     Octree_insert( Octree *tree, Vector3 position, double mass );
void    Octree_refresh_interior( Octree *tree );
Vector3 Octree_force( Octree *tree, Vector3 position, double mass );
void    Octree_destroy( Octree *tree );

#endif
