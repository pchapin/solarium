import sbt._

// See: https://www.scala-sbt.org/1.x/docs/Organizing-Build.html
object Dependencies {

  // Versions
  lazy val scalaTestVersion = "3.2.17"

  // Libraries
  val scalactic     = "org.scalactic" %% "scalactic" % scalaTestVersion
  val scalaTest     = "org.scalatest" %% "scalatest" % scalaTestVersion

  // https://mvnrepository.com/artifact/org.scala-lang.modules/scala-parallel-collections
  val scalaParallel = "org.scala-lang.modules" %% "scala-parallel-collections" % "1.0.4"

  // Projects
  val solariumDeps = Seq(scalaParallel, scalactic, scalaTest % Test)
}
