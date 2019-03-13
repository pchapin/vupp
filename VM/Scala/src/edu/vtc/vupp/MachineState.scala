package edu.vtc.vupp

/**
 * A case class that represents the state of the machine.
 * @param memory A map from memory location (16 bit addresses) to memory contents (16 bit).
 * @param registers A map from register number (0 .. 7) to register contents (16 bit).
 * @param pc The current program counter value (16 bit).
 * @param zf The current Z flag.
 * @param cf The current C flag.
 * @param nf The current interrupt flag. True means interrupts are enabled.
 * @param status One of: Running, Aborted, Halted.
 * @param history A list of messages related to the simulation, newest message first.
 */
case class MachineState(
   memory   : Map[Int, Int],
   registers: Map[Int, Int],
   pc       : Int,
   zf       : Boolean,
   cf       : Boolean,
   nf       : Boolean,   // Can't use 'if' because it's a reserved word!
   status   : MachineState.Run.Value,
   history  : List[String])

object MachineState {
  object Run extends Enumeration {
    val Running: Run.Value = Value  // The machine is executing (or could be).
    val Aborted: Run.Value = Value  // The machine encountered an unrecoverable error.
    val Halted : Run.Value = Value  // The machine executed a HALT instruction.
  }
}
