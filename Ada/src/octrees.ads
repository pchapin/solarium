---------------------------------------------------------------------------
-- FILE    : octrees.ads
-- SUBJECT : Specification of a package for handling octrees.
-- AUTHOR  : (C) Copyright 2024 by Peter Chapin
--
-- Please send comments or bug reports to
--
--      Peter Chapin <spicacality@kelseymountain.org>
---------------------------------------------------------------------------
with Ada.Finalization;
with Intervals;
with Vector3s;

generic
   type Numeric_Type is digits <>;
   with package Numeric_Vectors is new Vector3s(Numeric_Type);
   with package Numeric_Intervals is new Intervals(Numeric_Type);
package Octrees is
   -- Masses can't be negative. Tell the type system.
   subtype Mass_Type is Numeric_Type range 0.0 .. Numeric_Type'Last;

   type Octree is tagged limited private;

   -- Returns an initialized Octree that covers the specified region.
   function Make(Overall_Region : Numeric_Intervals.Box) return Octree;

   -- Returns the overall region of the given Octree.
   function Get_Overall_Region(Tree : Octree) return Numeric_Intervals.Box
     with Inline;

   -- Inserts the given mass at the specified position into the given Octree.
   procedure Insert(Tree : in out Octree; Position : in Numeric_Vectors.Vector3; Mass : in Mass_Type)
     with Pre => Numeric_Intervals.Contains
       (Tree.Get_Overall_Region,
        Numeric_Vectors.Get_X(Position),
        Numeric_Vectors.Get_Y(Position),
        Numeric_Vectors.Get_Z(Position));

   -- Updates node masses and centers of mass of all interior nodes up to and including the root node.
   procedure Refresh_Interior(Tree : in out Octree);

   -- Returns the gravitational force on the given mass at the specified position due to the given Octree.
   function Force(Tree : Octree; Position : Numeric_Vectors.Vector3; Mass : Mass_Type) return Numeric_Vectors.Vector3;

private
   type Octree_Node;
   type Octree_Node_Access is access Octree_Node;

   subtype Octant_Index_Type is Natural range 1 .. 8;
   type Octant_Array is array(Octant_Index_Type) of Octree_Node_Access;

   type Octree_Node is
      record
         Region         : Numeric_Intervals.Box;
         Total_Mass     : Mass_Type;
         Center_Of_Mass : Numeric_Vectors.Vector3;
         Octants        : Octant_Array;
         Is_Leaf        : Boolean;
      end record;

   type Octree is new Ada.Finalization.Limited_Controlled with
      record
         Root           : Octree_Node_Access;
         Overall_Region : Numeric_Intervals.Box;
      end record;

   overriding procedure Finalize(Tree : in out Octree);

end Octrees;
