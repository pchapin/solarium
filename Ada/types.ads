with Vector3s;

package Types is

   type Compute_Type is digits 16;

   package Compute_Vectors is new Vector3s(Compute_Type);
   use Compute_Vectors;

end Types;
