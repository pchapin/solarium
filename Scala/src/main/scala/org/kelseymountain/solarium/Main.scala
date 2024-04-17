package org.kelseymountain.solarium

import scala.collection.immutable.Vector
import scala.collection.parallel.CollectionConverters._

object Main {

  def main(args: Array[String]): Unit = {
    println("Hello, World")

    val myVector = Vector(0, 1, 2, 3, 4, 5, 6, 7, 8, 9)
    val resultVector1 = myVector map { _ + 1 }
    println(resultVector1)

    val myParallelVector = myVector.par
    val resultVector2 = myParallelVector map { _ + 1 }
    println(resultVector2)
  }

}
