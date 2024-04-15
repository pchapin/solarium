---------------------------------------------------------------------------
-- FILE    : solarium.adb
-- SUBJECT : Main program of the solarium application.
-- AUTHOR  : (C) Copyright 2020s by Peter C. Chapin
--
-- Please send comments or bug reports to
--
--      Peter C. Chapin <pchapin@vtc.edu>
---------------------------------------------------------------------------
with Ada.Text_IO;
with Simulator;

procedure Solarium is

   task type Dynamics_Computer_Type is
      entry Start_On_Range(Start_Object : in Positive; Stop_Object : in Positive);
   end Dynamics_Computer_Type;

   task body Dynamics_Computer_Type is
      Start : Positive;
      Stop  : Positive;
   begin
      accept Start_On_Range(Start_Object : in Positive; Stop_Object : in Positive) do
         Start := Start_Object;
         Stop  := Stop_Object;
      end Start_On_Range;

      Simulator.Time_Step(Start, Stop);
   end Dynamics_Computer_Type;


begin
   Ada.Text_IO.Put_Line("Hello, World!");
   for Time_Step_Count in 1 .. 8766 loop
      declare
         Dynamics_Computers : array(1 .. 4) of Dynamics_Computer_Type;
      begin
         Dynamics_Computers(1).Start_On_Range(   1,  2499);
         Dynamics_Computers(2).Start_On_Range(2500,  4999);
         Dynamics_Computers(3).Start_On_Range(5000,  7499);
         Dynamics_Computers(4).Start_On_Range(7500, 10000);
      end;
      Simulator.Swap_Dynamics;
   end loop;
end Solarium;
