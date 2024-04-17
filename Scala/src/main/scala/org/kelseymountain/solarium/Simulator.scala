package org.kelseymountain.solarium

import scala.collection.parallel.CollectionConverters._

object Simulator {

  private final val G = 6.673E-11   // Gravitational constant in MKS units
  private final val TIMESTEP = 3600 // Seconds

  def timeStep(
    objects        : Seq[SolarSystemObject],
    currentDynamics: Seq[ObjectDynamics]): Seq[ObjectDynamics] = {

    def computeObjectIDynamics(object_i: Int): ObjectDynamics = {

      // Consider the interaction with all other objects...
      val forceSequence = for (object_j <- objects.indices) yield {
        if (object_i != object_j) {
          val displacement = currentDynamics(object_j).position - currentDynamics(object_i).position
          val distanceSquared = displacement.magnitudeSquared
          val distance = Math.sqrt(distanceSquared)
          val forceMagnitude = (G * objects(object_i).mass * objects(object_j).mass) / distanceSquared
          (forceMagnitude / distance) * displacement
        }
        else {
          // We'll ignore the case of object_i == object_j by just including zero force in the sequence.
          Vector3(0.0, 0.0, 0.0)
        }
      }

      val totalForce = forceSequence.reduce(_ + _)

      // The total force is known. Now compute the effect on the dynamics of object_i.
      val acceleration = totalForce / objects(object_i).mass
      val deltaV = TIMESTEP * acceleration
      val deltaPosition = TIMESTEP * currentDynamics(object_i).velocity

      // The result for object_i
      ObjectDynamics(
        position = currentDynamics(object_i).position + deltaPosition,
        velocity = currentDynamics(object_i).velocity + deltaV)
    }

    // Do the computation for all objects!
    objects.indices map computeObjectIDynamics

    // Do it in parallel!
    //val parallelIndexRange = objects.indices.par
    //parallelIndexRange map computeObjectIDynamics
  }

}
