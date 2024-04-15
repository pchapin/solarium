with Ada.Numerics.Generic_Elementary_Functions;
with Types;

package body Simulator is
   use Types.Compute_Vectors;
   use type Types.Compute_Type;

   package Elementary_Compute_Functions is
     new Ada.Numerics.Generic_Elementary_Functions(Types.Compute_Type);
   use Elementary_Compute_Functions;

   type Object is
      record
         Mass : Types.Compute_Type;
      end record;

   type Object_Dynamics is
      record
         Position : Vector3;
         Velocity : Vector3;
      end record;

   type Object_Array is array(Positive range <>) of Object;
   type Object_Dynamics_Array is array(Positive range <>) of Object_Dynamics;

   Objects          : Object_Array(1 .. Constants.Object_Count);
   Current_Dynamics : Object_Dynamics_Array(1 .. Constants.Object_Count);
   Next_Dynamics    : Object_Dynamics_Array(1 .. Constants.Object_Count);


   procedure Time_Step(Start_Object : in Positive; Stop_Object : in Positive) is
      Force            : Vector3;             -- Force between two objects.
      Total_Force      : Vector3;             -- Total force on one object due to all the others.
      Displacement     : Vector3;             -- Vector displacement between two objects.
      Distance         : Types.Compute_Type;  -- Scalar distance between two objects.
      Distance_Squared : Types.Compute_Type;
      Force_Magnitude  : Types.Compute_Type;  -- Magnitude of the gravitational force.
      Acceleration     : Vector3;
      Delta_V          : Vector3;
      Delta_Position   : Vector3;
   begin
      -- For each object...
      for I in Start_Object .. Stop_Object loop
         Total_Force := Make(0.0, 0.0, 0.0);

         -- Consider the interactions with all other objects...
         for J in 1 .. Constants.Object_Count loop
            if I /= J then
               Displacement     := Current_Dynamics(J).Position - Current_Dynamics(I).Position;
               Distance_Squared := Magnitude_Squared(Displacement);
               Distance         := Sqrt(Distance_Squared);
               Force_Magnitude  := (Constants.G * Objects(I).Mass * Objects(J).Mass)/Distance_Squared;
               Force            := (Force_Magnitude / Distance) * Displacement;
               Total_Force      := Total_Force + Force;
            end if;
         end loop;

         -- The total force is known. Now compute the effect on the dynamics of object I.
         Acceleration := Total_Force / Objects(I).Mass;
         Delta_V := Constants.Time_Step * Acceleration;
         Delta_Position := Constants.Time_Step * Current_Dynamics(I).Velocity;
         Next_Dynamics(I).Velocity := Current_Dynamics(I).Velocity + Delta_V;
         Next_Dynamics(I).Position := Current_Dynamics(I).Position + Delta_Position;
      end loop;

   end Time_Step;


   procedure Swap_Dynamics is
      Temp : Object_Dynamics;
   begin
      for I in 1 .. Constants.Object_Count loop
         Temp                := Current_Dynamics(I);
         Current_Dynamics(I) := Next_Dynamics(I);
         Next_Dynamics(I)    := Temp;
      end loop;
   end Swap_Dynamics;


end Simulator;
