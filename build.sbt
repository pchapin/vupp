import Dependencies._

enablePlugins(Antlr4Plugin)

ThisBuild / organization  := "edu.vtc"
ThisBuild / version       := "0.1.0-SNAPSHOT"
ThisBuild / scalaVersion  := "2.13.8"   // Think about upgrading to Scala 3.1.0. Check libs!
ThisBuild / scalacOptions :=
  Seq("-encoding", "UTF-8", // Encoding of the source files.
      "-feature",
      "-deprecation",       // Tell us about deprecated things.
      "-unchecked",
      "-Wunused:nowarn",    // Warn if the nowarn annotation doesn't actually suppress a warning.
      "-Xsource:3",         // Help us migrate to Scala 3 by forbidding somethings and allowing others.
      "-Ywarn-dead-code",
      "-Ywarn-value-discard")

Test / logBuffered := false

// A master project...
lazy val vupp = (project in file("."))
  .settings(
    name := "VuPP",
    // libraryDependencies ++= vuppDeps
  )
  .dependsOn(fink, llvupp, vas, vm, vocal)


lazy val fink = (project in file("Fink"))
  .settings(
    name := "Fink"
    // libraryDependencies ++= vuppDeps
  )

lazy val llvupp = (project in file("LLVuPP"))
  .settings(
    name := "LLVuPP"
    // libraryDependencies ++= vuppDeps
  )

lazy val vas = (project in file("VAS"))
  .settings(
    name := "VAS"
    // libraryDependencies ++= vuppDeps
  )

lazy val vm = (project in file("VuPPVM"))
  .settings(
    name := "VuPPVM",
    libraryDependencies ++= vuppVMDeps
  )

lazy val vocal = (project in file("Vocal"))
  .settings(
    name := "Vocal",
    // libraryDependencies ++= vuppDeps

    Antlr4 / antlr4Version     := "4.9.2",
    Antlr4 / antlr4PackageName := Some("edu.vtc.vupp"),
    Antlr4 / antlr4GenListener := true,
    Antlr4 / antlr4GenVisitor  := true
  )
