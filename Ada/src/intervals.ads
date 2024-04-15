---------------------------------------------------------------------------
-- FILE    : intervals.ads
-- SUBJECT : Specification of a package for handling dynamically ranged values in several dimensions.
-- AUTHOR  : (C) Copyright 2020 by Peter C. Chapin
--
-- Please send comments or bug reports to
--
--      Peter C. Chapin <pchapin@vtc.edu>
---------------------------------------------------------------------------

generic
   type Numeric_Type is digits <>;
package Intervals is

   ----------------------------
   -- One Dimensional Intervals
   ----------------------------

   type Interval is private;

   function Make(Lower, Upper : Numeric_Type) return Interval
     with Inline, Pre => (Lower <= Upper);

   function Lower(I : Interval) return Numeric_Type
     with Inline;

   function Upper(I : Interval) return Numeric_Type
     with Inline;

   -- Returns the size of the interval.
   function Size(I : Interval) return Numeric_Type
     with Inline, Post => Size'Result >= 0.0;

   -- Returns True if the given coordinate is inside the given interval.
   function Contains(I : Interval; X : Numeric_Type) return Boolean;


   ----------------------------
   -- Two Dimensional Intervals
   ----------------------------

   type Rectangle is private;

   function Make(X_Range, Y_Range : Interval) return Rectangle
     with Inline;

   function X_Range(R : Rectangle) return Interval
     with Inline;

   function Y_Range(R : Rectangle) return Interval
     with Inline;

   -- Returns True if the given coordinates are inside the given rectangle.
   function Contains(R : Rectangle; X, Y : Numeric_Type) return Boolean;


   ------------------------------
   -- Three Dimensional Intervals
   ------------------------------

   type Box is private;

   function Make(X_Range, Y_Range, Z_Range : Interval) return Box
     with Inline;

   function X_Range(B : Box) return Interval
     with Inline;

   function Y_Range(B : Box) return Interval
     with Inline;

   function Z_Range(B : Box) return Interval
     with Inline;

   -- Returns True if the given coordinates are inside the given box.
   function Contains(B : Box; X, Y, Z : Numeric_Type) return Boolean;


private

   type Interval is
      record
         Min : Numeric_Type;
         Max : Numeric_Type;
      end record;

   type Rectangle is
      record
         X_Interval : Interval;
         Y_Interval : Interval;
      end record;

   type Box is
      record
         X_Interval : Interval;
         Y_Interval : Interval;
         Z_Interval : Interval;
      end record;

end Intervals;
