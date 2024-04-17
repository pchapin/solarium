package org.kelseymountain.solarium

import scala.collection.parallel.CollectionConverters._

object Main {

  private def parallelSample(): Unit = {
    val myVector = Vector(0, 1, 2, 3, 4, 5, 6, 7, 8, 9)
    val resultVector1 = myVector map { _ + 1 }
    println(resultVector1)

    val myParallelVector = myVector.par
    val resultVector2 = myParallelVector map { _ + 1 }
    println(resultVector2)
  }

  private def vector3Sample(): Unit = {
    val v1 = Vector3(1.0, 2.0, 3.0)
    val v2 = 3 * v1
    println(s"(${v2.x}, ${v2.y}, ${v2.z})")
  }

  def main(args: Array[String]): Unit = {
    println("Hello, World!")
    parallelSample()
    vector3Sample()
  }

}
