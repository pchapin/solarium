import Dependencies._

ThisBuild / version := "0.1.0-SNAPSHOT"
ThisBuild / scalaVersion := "2.13.12"
ThisBuild / scalacOptions :=
  Seq("-encoding", "UTF-8",
      "-feature",
      "-deprecation",
      "-unchecked",
      "-Wunused:nowarn")    // Warn if the nowarn annotation doesn't actually suppress a warning.

Test / logBuffered := false

lazy val solarium = (project in file("."))
  .settings(
    name := "Solarium",
    libraryDependencies ++= solariumDeps
  )

