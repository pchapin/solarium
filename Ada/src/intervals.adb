---------------------------------------------------------------------------
-- FILE    : intervals.adb
-- SUBJECT : Implementation of a package for handling dynamically ranged values in several dimensions.
-- AUTHOR  : (C) Copyright 2024 by Peter Chapin
--
-- Please send comments or bug reports to
--
--      Peter Chapin <spicacality@kelseymountain.org>
---------------------------------------------------------------------------

package body Intervals is

   ----------------------------
   -- One Dimensional Intervals
   ----------------------------

   function Make(Lower, Upper : Numeric_Type) return Interval is
   begin
      return (Min => Lower, Max => Upper);
   end Make;

   function Lower(I : Interval) return Numeric_Type is
   begin
      return I.Min;
   end Lower;

   function Upper(I : Interval) return Numeric_Type is
   begin
      return I.Max;
   end Upper;

   function Size(I : Interval) return Numeric_Type is
   begin
      return Max - Min;
   end Size;

   function Contains(I : Interval; X : Numeric_Type) return Boolean is
   begin
      return I.Min <= X and X <= I.Max;
   end Contains;


   ----------------------------
   -- Two Dimensional Intervals
   ----------------------------

   function Make(X_Range, Y_Range : Interval) return Rectangle is
   begin
      return (X_Interval => X_Range, Y_Interval => Y_Range);
   end Make;

   function X_Range(R : Rectangle) return Interval is
   begin
      return R.X_Interval;
   end X_Range;

   function Y_Range(R : Rectangle) return Interval is
   begin
      return R.Y_Interval;
   end Y_Range;

   function Contains(R : Rectangle; X, Y : Numeric_Type) return Boolean is
   begin
      return Contains(R.X_Interval, X) and Contains(R.Y_Interval, Y);
   end Contains;


   ------------------------------
   -- Three Dimensional Intervals
   ------------------------------

   function Make(X_Range, Y_Range, Z_Range : Interval) return Box is
   begin
      return (X_Interval => X_Range, Y_Interval => Y_Range, Z_Interval => Z_Range);
   end Make;

   function X_Range(B : Box) return Interval is
   begin
      return B.X_Interval;
   end X_Range;

   function Y_Range(B : Box) return Interval is
   begin
      return B.Y_Interval;
   end Y_Range;

   function Z_Range(B : Box) return Interval is
   begin
      return B.Z_Interval;
   end Z_Range;

   function Contains(B : Box; X, Y, Z : Numeric_Type) return Boolean is
   begin
      return Contains(B.X_Interval, X) and Contains(B.Y_Interval, Y) and Contains(B.Z_Interval, Z);
   end Contains;

end Intervals;
