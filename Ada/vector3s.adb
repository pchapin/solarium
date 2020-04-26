---------------------------------------------------------------------------
-- FILE    : vector3s.adb
-- SUBJECT : Body of a package for handling three dimensional vectors.
-- AUTHOR  : (C) Copyright 2020 by Peter C. Chapin
--
-- Please send comments or bug reports to
--
--      Peter C. Chapin <pchapin@vtc.edu>
---------------------------------------------------------------------------

package body Vector3s is

   function Make(X, Y, Z : Numeric_Type) return Vector3 is
   begin
      return (X, Y, Z);
   end Make;


   function Get_X(V : Vector3) return Numeric_Type is
   begin
      return V(1);
   end Get_X;


   function Get_Y(V : Vector3) return Numeric_Type is
   begin
      return V(2);
   end Get_Y;


   function Get_Z(V : Vector3) return Numeric_Type is
   begin
      return V(3);
   end Get_Z;


   function "+"(L, R : Vector3) return Vector3 is
   begin
      return (L(1) + R(1), L(2) + R(2), L(3) + R(3));
   end "+";


   function "-"(L, R : Vector3) return Vector3 is
   begin
      return (L(1) - R(1), L(2) - R(2), L(3) - R(3));
   end "-";


   function "*"(L : Numeric_Type; R : Vector3) return Vector3 is
   begin
      return (L * R(1), L * R(2), L * R(3));
   end "*";


   function "/"(L : Vector3; R : Numeric_Type) return Vector3 is
   begin
      return (L(1)/R, L(2)/R, L(3)/R);
   end "/";


   function Magnitude_Squared(Vector : Vector3) return Numeric_Type is
   begin
      return Vector(1) * Vector(1) +  Vector(2) * Vector(2) + Vector(3) * Vector(3);
   end Magnitude_Squared;


end Vector3s;
