/*! \file Octree.h
 *  \brief Declarations of octrees and their operations.
 *  \author Peter C. Chapin <PChapin@vtc.vsc.edu>
 *  \date October 22, 2012
 */

#ifndef OCTREE_H
#define OCTREE_H

#include "Interval.h"
#include "Vector3.h"

struct OctreeNode {
    /*@null@*/ /*@owned@*/ struct OctreeNode *octants[8];
    /*@shared@*/           int     is_leaf;
                           Box     region;
                           Vector3 center_of_mass;
                           double  total_mass;
};

typedef /*@abstract@*/ struct {
    /*@null@*/ /*@owned@*/ struct OctreeNode *root;
                           Box    overall_region;
} Octree;

void    Octree_init( /*@out@*/ Octree *tree, Box *overall_region );
int     Octree_insert( Octree *tree, Vector3 position, double mass );
void    Octree_refresh_interior( Octree *tree );
Vector3 Octree_force( Octree *tree, Vector3 position, double mass );
void    Octree_destroy( Octree *tree );

#endif
