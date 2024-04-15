with Constants;

package Simulator is

   procedure Time_Step(Start_Object : in Positive; Stop_Object : in Positive)
     with
       Pre =>
         Start_Object in 1 .. Constants.Object_Count and
         Stop_Object  in 1 .. Constants.Object_Count and
         Start_Object <= Stop_Object;

   procedure Swap_Dynamics;

end Simulator;
