---------------------------------------------------------------------------
-- FILE    : octrees.adb
-- SUBJECT : Body of a package for handling octrees.
-- AUTHOR  : (C) Copyright 2024 by Peter Chapin
--
-- Please send comments or bug reports to
--
--      Peter Chapin <spicacality@kelseymountain.org>
---------------------------------------------------------------------------
with Ada.Numerics.Generic_Elementary_Functions;
with Ada.Unchecked_Deallocation;

package body Octrees is
   package Numeric_Functions is new Ada.Numerics.Generic_Elementary_Functions(Numeric_Type);

   -- The universal gravitational constant in MKS units.
   G : constant := 6.67384E-11;


   ----------------------
   -- Visible Subprograms
   ----------------------

   function Make(Overall_Region : Numeric_Intervals.Box) return Octree is
   begin
      return Result : Octree do
         Result.Root := null;
         Result.Overall_Region := Overall_Region;
      end return;
   end Make;


   function Get_Overall_Region(Tree : Octree) return Numeric_Intervals.Box is (Tree.Overall_Region);


   procedure Insert(Tree : in out Octree; Position : in Numeric_Vectors.Vector3; Mass : in Mass_Type) is
      use Numeric_Vectors;
      use Numeric_Intervals;

      procedure Subtree_Insert(Current : in not null Octree_Node_Access; New_Node : in not null Octree_Node_Access)
        with Pre => New_Node.Is_Leaf
      is

         -- Given the center of pass of a particular leaf, return the octant index of the octant that would contain that node.
         function Get_Octant
           (Overall : in not null Octree_Node_Access; Particular : in not null Octree_Node_Access) return Octant_Index_Type
           with Pre => Particular.Is_Leaf
         is

            Center_X, Center_Y, Center_Z : Numeric_Type;
            X, Y, Z : Numeric_Type;
         begin
            Center_X := (Upper(X_Range(Overall.Region)) + Lower(X_Range(Overall.Region))) / 2.0;
            Center_Y := (Upper(Y_Range(Overall.Region)) + Lower(Y_Range(Overall.Region))) / 2.0;
            Center_Z := (Upper(Z_Range(Overall.Region)) + Lower(Z_Range(Overall.Region))) / 2.0;
            X := Get_X(Particular.Center_Of_Mass) - Center_X;
            Y := Get_Y(Particular.Center_Of_Mass) - Center_Y;
            Z := Get_Z(Particular.Center_Of_Mass) - Center_Z;

            if X >= 0.0 and Y >= 0.0 and Z >= 0.0 then return 1; end if;
            if X >= 0.0 and Y >= 0.0 and Z <  0.0 then return 2; end if;
            if X >= 0.0 and Y <  0.0 and Z >= 0.0 then return 3; end if;
            if X >= 0.0 and Y <  0.0 and Z <  0.0 then return 4; end if;
            if X <  0.0 and Y >= 0.0 and Z >= 0.0 then return 5; end if;
            if X <  0.0 and Y >= 0.0 and Z <  0.0 then return 6; end if;
            if X <  0.0 and Y <  0.0 and Z >= 0.0 then return 7; end if;
            -- if X <  0.0 and Y <  0.0 and Z <  0.0 then return 8; end if;
            return 8;  -- This is the only remaining possibility.
                       -- Leaving this out of an if statement removes a warning.
         end Get_Octant;

         -- Given an overall node, return the region associated with a particular octant index.
         function Get_Region
           (Overall : in not null Octree_Node_Access; Octant_Index : Octant_Index_Type) return Box
         is

            Center_X, Center_Y, Center_Z : Numeric_Type;
            X_Interval : Interval     := X_Range(Overall.Region);
            Y_Interval : Interval     := Y_Range(Overall.Region);
            Z_Interval : Interval     := Z_Range(Overall.Region);
            X_Lower    : Numeric_Type := Lower(X_Interval);
            X_Upper    : Numeric_Type := Upper(X_Interval);
            Y_Lower    : Numeric_Type := Lower(Y_Interval);
            Y_Upper    : Numeric_Type := Upper(Y_Interval);
            Z_Lower    : Numeric_Type := Lower(Z_Interval);
            Z_Upper    : Numeric_Type := Upper(Z_Interval);
            Subregion  : Box;
         begin
            Center_X := (Upper(X_Range(Overall.Region)) + Lower(X_Range(Overall.Region))) / 2.0;
            Center_Y := (Upper(Y_Range(Overall.Region)) + Lower(Y_Range(Overall.Region))) / 2.0;
            Center_Z := (Upper(Z_Range(Overall.Region)) + Lower(Z_Range(Overall.Region))) / 2.0;
            case Octant_Index is
            when 1 =>
               Subregion := Make
                 (Make(Center_X, X_Upper ),
                  Make(Center_Y, Y_Upper ),
                  Make(Center_Z, Z_Upper ));
            when 2 =>
               Subregion := Make
                 (Make(Center_X, X_Upper ),
                  Make(Center_Y, Y_Upper ),
                  Make(Z_Lower,  Center_Z));
            when 3 =>
               Subregion := Make
                 (Make(Center_X, X_Upper ),
                  Make(Y_Lower,  Center_Y),
                  Make(Center_Z, Z_Upper ));
            when 4 =>
               Subregion := Make
                 (Make(Center_X, X_Upper ),
                  Make(Y_Lower,  Center_Y),
                  Make(Z_Lower,  Center_Z));
            when 5 =>
               Subregion := Make
                 (Make(X_Lower,  Center_X),
                  Make(Center_Y, Y_Upper ),
                  Make(Center_Z, Z_Upper ));
            when 6 =>
               Subregion := Make
                 (Make(X_Lower,  Center_X),
                  Make(Center_Y, Y_Upper ),
                  Make(Z_Lower,  Center_Z));
            when 7 =>
               Subregion := Make
                 (Make(X_Lower,  Center_X),
                  Make(Y_Lower,  Center_Y),
                  Make(Center_Z, Z_Upper ));
            when 8 =>
               Subregion := Make
                 (Make(X_Lower,  Center_X),
                  Make(Y_Lower,  Center_Y),
                  Make(Z_Lower,  Center_Z));
            end case;
            return Subregion;
         end Get_Region;

         Octent_Index : Octant_Index_Type;
         Subnode : Octree_Node_Access;
      begin
         -- If Current is not a leaf node...
         if not Current.Is_Leaf then
            Octent_Index := Get_Octant(Current, New_Node);
            if Current.Octants(Octent_Index) = null then
               New_Node.Region := Get_Region(Current, Octent_Index);
               Current.Octants(Octent_Index) := New_Node;
            else
               Subtree_Insert(Current.Octants(Octent_Index), New_Node);
            end if;
         else
            -- Subdivide the current node.
            Subnode := new Octree_Node;
            Subnode.all := Current.all;
            Current.Is_Leaf := False;
            Subtree_Insert(Current, Subnode);
            Subtree_Insert(Current, New_Node);
         end if;
      end Subtree_Insert;

      New_Node : Octree_Node_Access;

   begin
      New_Node := new Octree_Node;
      New_Node.Octants := (others => null);
      New_Node.Is_Leaf := True;
      New_Node.Center_Of_Mass := Position;
      New_Node.Total_Mass := Mass;
      -- New_Node.Region will be defined later.

      -- Add it to the tree.
      if Tree.Root = null then
         New_Node.Region := Tree.Overall_Region;
         Tree.Root := New_Node;
      else
         Subtree_Insert(Tree.Root, New_Node);
      end if;
   end Insert;


   procedure Refresh_Interior(Tree : in out Octree) is

      procedure Subtree_Refresh(Node : in not null Octree_Node_Access) is
         use type Numeric_Vectors.Vector3;

         Temp : Numeric_Vectors.Vector3 := Numeric_Vectors.Make(0.0, 0.0, 0.0);
      begin
         if Node.Is_Leaf then return; end if;

         Node.Total_Mass := 0.0;
         for I in Octant_Index_Type loop
            if Node.Octants(I) /= null then
               Subtree_Refresh(Node.Octants(I));
               Node.Total_Mass := Node.Total_Mass + Node.Octants(I).Total_Mass;
               Temp := Temp + (Node.Octants(I).Total_Mass * Node.Octants(I).Center_Of_Mass);
            end if;
         end loop;
         Node.Center_Of_Mass := Temp / Node.Total_Mass;
      end Subtree_Refresh;

   begin
      if Tree.Root /= null then
         Subtree_Refresh(Tree.Root);
      end if;
   end Refresh_Interior;


   function Force
     (Tree : Octree; Position : Numeric_Vectors.Vector3; Mass : Mass_Type) return Numeric_Vectors.Vector3
   is
      use type Numeric_Vectors.Vector3;
      use Numeric_Intervals;

      Force : Numeric_Vectors.Vector3 := Numeric_Vectors.Make(0.0, 0.0, 0.0);

      function Subtree_Force(Node : in not null Octree_Node_Access) return Numeric_Vectors.Vector3 is
         Theta            : constant := 0.5;
         Displacement     : Numeric_Vectors.Vector3;  -- Vector connecting the given mass to the node's center of mass.
         Distance         : Numeric_Type;       -- The scalar distance between the given mass and the node's center of mass.
         Distance_Squared : Numeric_Type;
         Size_X, Size_Y, Size_Z : Numeric_Type; -- The X, Y, and Z sizes of the node's region.
         Size_Measure     : Numeric_Type;       -- The largest of the X, Y, and Z sizes of the node's region.
         Force_Magnitude  : Numeric_Type;
         Force            : Numeric_Vectors.Vector3 := Numeric_Vectors.Make(0.0, 0.0, 0.0);
      begin
         -- Ignore this node if it is the node containing the object under consideration.
         if Node.Is_Leaf and Node.Center_Of_Mass = Position then
            return Force;
         end if;

         Displacement := Node.Center_Of_Mass - Position;
         Distance_Squared := Numeric_Vectors.Magnitude_Squared(Displacement);
         Distance := Numeric_Functions.Sqrt(Distance_Squared);
         Size_X := Size(X_Range(Node.Region));
         Size_Y := Size(Y_Range(Node.Region));
         Size_Z := Size(Z_Range(Node.Region));

         -- Compute the maximum of Size_X, Size_Y, Size_Z.
         Size_Measure := Size_X;
         if Size_Y > Size_Measure then
            Size_Measure := Size_Y;
         end if;
         if Size_Z > Size_Measure then
            Size_Measure := Size_Z;
         end if;

         -- If this region is a leaf or "far enough" away, do the direct computation.
         if Node.Is_Leaf or Size_Measure/Distance < Theta then
            Force_Magnitude := G * Mass * Node.Total_Mass / Distance_Squared;
            Force := (Force_Magnitude / Distance) * Displacement;
         else
            -- Otherwise recursively examine the octants.
            for I in Octant_Index_Type loop
               if Node.Octants(I) /= null then
                  Force := Force + Subtree_Force(Node.Octants(I));
               end if;
            end loop;
         end if;

         return Force;
      end Subtree_Force;

   begin
      if Tree.Root /= null then
         Force := Subtree_Force(Tree.Root);
      end if;
      return Force;
   end Force;


   -----------------------
   -- Private Subprograms
   ----------------------

   procedure Free_Node is new Ada.Unchecked_Deallocation(Octree_Node, Octree_Node_Access);

   procedure Finalize(Tree : in out Octree) is

      procedure Subtree_Destroy(Node : in out Octree_Node_Access) is
      begin
         for I in Octant_Index_Type loop
            if Node.Octants(I) /= null then
               Subtree_Destroy(Node.Octants(I));
            end if;
         end loop;
         Free_Node(Node);
      end Subtree_Destroy;

   begin
      if Tree.Root /= null then
         Subtree_Destroy(Tree.Root);
      end if;
   end Finalize;


end Octrees;
