---------------------------------------------------------------------------
-- FILE    : vector3s.ads
-- SUBJECT : Specification of a package for handling three dimensional vectors.
-- AUTHOR  : (C) Copyright 2014 by Peter C. Chapin
--
-- Please send comments or bug reports to
--
--      Peter C. Chapin <PChapin@vtc.vsc.edu>
---------------------------------------------------------------------------

generic
   type Numeric_Type is digits <>;
package Vector3s is

   type Vector3 is private;

   function Make(X, Y, Z : Numeric_Type) return Vector3
     with Inline;


   -- Accessors
   ------------

   function Get_X(V : Vector3) return Numeric_Type
     with Inline;

   function Get_Y(V : Vector3) return Numeric_Type
     with Inline;

   function Get_Z(V : Vector3) return Numeric_Type
     with Inline;


   -- Vector Operations
   --------------------

   function "+"(L, R : Vector3) return Vector3
     with Inline;

   function "-"(L, R : Vector3) return Vector3
     with Inline;

   function "*"(L : Numeric_Type; R : Vector3) return Vector3
     with Inline;

   function "/"(L : Vector3; R : Numeric_Type) return Vector3
     with Inline;

   function Magnitude_Squared(Vector : Vector3) return Numeric_Type
     with Inline;

private

   -- Holds the (X, Y, Z) components of the vector in that order.
   type Vector3 is array(1 .. 3) of Numeric_Type;

end Vector3s;
