import Dependencies._

ThisBuild / organization := "org.pchapin"
ThisBuild / version      := "0.1.0-SNAPSHOT"
ThisBuild / scalaVersion := "2.12.8"

logBuffered in Test := false

lazy val augusta = (project in file("."))
  .settings(
    name := "VuPPVM",
    libraryDependencies ++= vuppDeps,
  )
