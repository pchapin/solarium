with Types;

package Constants is
   use Types;

   Object_Count : constant := 10_000;
   AU           : constant Compute_Type := 1.49597870700E+11;  -- Meters per astronomical unit.
   Average_Velocity : constant Compute_Type := 2.9785E+3;      -- Meters per second.
   G            : constant Compute_Type := 6.673E-11;          -- Gravitational constant.
   Time_Step    : constant Compute_Type := 3.6E+03;            -- Seconds in a time step (one hour).
   -- The Average_Velocity above is 1/10 the magnitude of the velocity of Earth in its orbit.

end Constants;
