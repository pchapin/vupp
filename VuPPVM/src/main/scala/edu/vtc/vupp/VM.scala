package edu.vtc.vupp

import scala.collection.immutable.TreeMap

object VM {

  /**
   * For demonstration purposes the main method simulates the execution of a trivial program
   * that counts a register down from 16 to 0. The program is hard coded. A more general version
   * of this application would read the VuPP program from a file.
   *
   * @param args The command line arguments.
   */
  def main(args: Array[String]): Unit = {

    // Initial program:
    //
    // 0000 | 04080010         copy 0x10, r0
    //      |          @sample_loop
    // 0002 | 2408             dec  r0
    // 0003 | 54000002         jnz  @sample_loop
    // 0005 | 7000             halt
    //
    val initialMemory = TreeMap(
      0x0000 -> 0x0408, 0x0001 -> 0x0010,
      0x0002 -> 0x2408,
      0x0003 -> 0x5400, 0x0004 -> 0x0002,
      0x0005 -> 0x7000)

    val initialRegisters = TreeMap[Int, Int]()
    val initialState = MachineState(
      memory    = initialMemory,      // Memory with program loaded; otherwise undefined.
      registers = initialRegisters,   // All registers initially undefined.
      pc        = 0x0000,             // Execution begins at location zero.
      zf        = false,              // Zero flag initially off (should be undefined?).
      cf        = false,              // Carry flag initially off (should be undefined?).
      nf        = false,              // Interrupt flag disabled on system start.
      status    = MachineState.Run.Running,
      history   = List()              // No messages yet.
    )

    // Run the program under simulation; display the results.
    displayState(Simulator.execute(initialState))
  }


  /**
   * Displays the state of the machine in a nice way. Both memory and registers are displayed.
   * This method produces the best results if the memory and register map inside the state have
   * ordered keys. It is recommended to use TreeMaps for that purpose. This method also displays
   * the history trace associated with the machine. That trace contains messages related to the
   * execution history of any simulation that has been done. This method assumes the most recent
   * message is at the head of the execution history.
   *
   * @param currentState The state to display.
   */
  def displayState(currentState: MachineState): Unit = {

    // Give nice names to the components of the state for easy use below.
    val MachineState(memory, registers, pc, zf, cf, nf, state, history) = currentState

    // Display final results.
    print("EXECUTION COMPLETE\n")
    print("==================\n")
    state match {
      case MachineState.Run.Running =>
        println("Error: The machine is still running!")
      case MachineState.Run.Aborted =>
        println("Error: Execution aborted!")
      case MachineState.Run.Halted =>
        println("Machine HALTed")
    }

    print("\n")
    print("History\n")
    print("-------\n")
    history.reverse foreach { println(_) }

    print("\n")
    print("Registers\n")
    print("---------\n")
    for ((register, value) <- registers) {
      printf("r%d = %04X\n", register, value)
    }
    printf("pc = %04X\n", pc)
    printf("zf = %d; cf = %d\n", if (zf) 1 else 0, if (cf) 1 else 0)

    print("\n")
    print("Memory\n")
    print("------\n")
    for ((memoryAddress, memoryContents) <- memory) {
      printf("%04X = %04X\n", memoryAddress, memoryContents)
    }
  }

}
