package org.kelseymountain.solarium

case class Vector3(x: Double, y: Double, z: Double) {

  def +(other: Vector3): Vector3 = Vector3(x + other.x, y + other.y, z + other.z)

  def -(other: Vector3): Vector3 = Vector3(x - other.x, y - other.y, z - other.z)

  def /(divisor: Double): Vector3 = Vector3(x/divisor, y/divisor, z/divisor)

  def magnitudeSquared: Double = x*x + y*y + z*z
}

object Vector3 {
  implicit class DoubleOps(d: Double) {
    def *(v: Vector3): Vector3 = Vector3(d * v.x, d * v.y, d * v.z)
  }
}
