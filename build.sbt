import Dependencies._

enablePlugins(Antlr4Plugin)

ThisBuild / organization  := "edu.vtc"
ThisBuild / version       := "0.1.0-SNAPSHOT"
ThisBuild / scalaVersion  := "2.12.8"
ThisBuild / scalacOptions :=
  Seq("-encoding", "UTF-8",
      "-feature",
      "-deprecation",
      "-unchecked",
      "-Ywarn-adapted-args",
      "-Ywarn-dead-code",
      "-Ywarn-infer-any",
      "-Ywarn-unused-import",
      "-Ywarn-value-discard")

logBuffered in Test := false

lazy val fink = (project in file("Fink"))
  .settings(
    name := "Fink"
    // libraryDependencies ++= vuppDeps
  )

lazy val vas = (project in file("VAS"))
  .settings(
    name := "VAS"
    // libraryDependencies ++= vuppDeps
  )

lazy val vm = (project in file("VM"))
  .settings(
    name := "VuPPVM",
    libraryDependencies ++= vuppVMDeps
  )

lazy val vocal = (project in file("Vocal"))
  .settings(
    name := "Vocal",
    // libraryDependencies ++= vuppDeps

    antlr4Version     in Antlr4 := "4.7.2",
    antlr4PackageName in Antlr4 := Some("edu.vtc.vupp"),
    antlr4GenListener in Antlr4 := true,
    antlr4GenVisitor  in Antlr4 := true
    //excludeFilter in unmanagedSources := HiddenFileFilter || "*slem*"
  )
