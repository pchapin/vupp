package edu.vtc.vupp

import scala.annotation.tailrec

object Simulator {

  /**
   * Return the addressing mode information embedded in a given opcode.
   * @param opcode The opcode to dismantle.
   * @return (source_mode, source_register, destination_mode, destination_register)
   */
  private def getModes(opcode: Int): (Int, Int, Int, Int) =
    ((opcode & 0x0300) >> 8,
     (opcode & 0x00E0) >> 5,
     (opcode & 0x0018) >> 3,
     (opcode & 0x0007) >> 0)


  /**
   * Check the opcode pointed at by the program counter to verify that it is a valid VuPP op-
   * code. If it is not abort the simulation.
   *
   * @param currentState The state to check.
   * @return The given state if the current opcode is okay, otherwise return an aborted state.
   */
  private def checkCurrentOpcode(currentState: MachineState): MachineState = {

    // The check functions return 'true' if the addressing modes and register selections are
    // appropriate for the class of instruction each covers. For example, checkModesNoOperands
    // deals with instructions that have no operands. The rules enforced are:
    //
    // 1. Destination operands can't use the immediate addressing mode.
    // 2. Immediate mode (0) does not require a register so the register bits must be zero.
    // 3. Indirect mode (2) does not require a register so the register bits must be zero.
    // 4. Immediate and Indirect modes require an additional word after the opcode.

    def checkModesNoOperands
      (sa: Int, sr: Int, da: Int, dr: Int, memory: Map[Int, Int], pc: Int): Boolean = {
      sa == 0 &&
      sr == 0 &&
      da == 0 &&
      dr == 0
    }

    def checkModesDestinationOnly
      (sa: Int, sr: Int, da: Int, dr: Int, memory: Map[Int, Int], pc: Int): Boolean = {
      sa == 0 &&
      sr == 0 &&
      da != 0 &&
      !(da == 2 && dr != 0) &&
      !(da == 2 && !memory.isDefinedAt(pc + 1))
    }

    def checkModesSourceOnly
      (sa: Int, sr: Int, da: Int, dr: Int, memory: Map[Int, Int], pc: Int): Boolean = {
      da == 0 &&
      dr == 0 &&
      !(sa == 0 && sr != 0) &&
      !(sa == 0 && !memory.isDefinedAt(pc + 1)) &&
      !(sa == 2 && sr != 0) &&
      !(sa == 2 && !memory.isDefinedAt(pc + 1))
    }

    def checkModesBoth
      (sa: Int, sr: Int, da: Int, dr: Int, memory: Map[Int, Int], pc: Int): Boolean = {
      !(sa == 0 && sr != 0) &&
      !(sa == 0 && !memory.isDefinedAt(pc + 1)) &&
      !(sa == 2 && sr != 0) &&
      !(sa == 2 && !memory.isDefinedAt(pc + 1)) &&
      da != 0 &&
      !(da == 2 && dr != 0) &&
      // Indirect requires an additional word. Location depends on if source needs a word also.
      !(da == 2 && !memory.isDefinedAt(pc + (if (sa == 0 || sa == 2) 2 else 1)))
    }

    // TODO: This array is really a constant. It could be moved out of the method.
    val checkFunctions = Array(
      checkModesNoOperands _,       // nop
      checkModesBoth _,             // copy
      checkModesBoth _,             // add
      checkModesBoth _,             // sub
      checkModesBoth _,             // cmp
      checkModesBoth _,             // and
      checkModesBoth _,             // or
      checkModesBoth _,             // xor
      checkModesDestinationOnly _,  // inc
      checkModesDestinationOnly _,  // dec
      checkModesDestinationOnly _,  // shl
      checkModesDestinationOnly _,  // shr
      checkModesDestinationOnly _,  // rtl
      checkModesDestinationOnly _,  // rtr
      checkModesSourceOnly _,       // call
      checkModesNoOperands _,       // ret
      checkModesNoOperands _,       // reti
      checkModesSourceOnly _,       // push
      checkModesDestinationOnly _,  // pop
      checkModesSourceOnly _,       // jmp
      checkModesSourceOnly _,       // jz
      checkModesSourceOnly _,       // jnz
      checkModesSourceOnly _,       // jc
      checkModesSourceOnly _,       // jnc
      checkModesNoOperands _,       // stc
      checkModesNoOperands _,       // clc
      checkModesNoOperands _,       // ei
      checkModesNoOperands _,       // di
      checkModesNoOperands _        // halt
    )

    val MachineState(memory, registers, pc, zf, cf, nf, status, history) = currentState
    require(status == MachineState.Run.Running, "checking opcode on a stopped machine")

    if (!memory.isDefinedAt(pc)) {
      val newHistory =
        "Execution of uninitialized memory at pc = %04X".format(pc) :: history
      MachineState(memory, registers, pc, zf, cf, nf, MachineState.Run.Aborted, newHistory)
    }
    else {
      val invalidHistory =
        "Invalid opcode %04X at pc = %04X".format(memory(pc), pc) :: history

      // TODO: Still need to check if bit 15 is one (if so, the opcode is invalid).
      val instruction = (memory(pc) & 0x7C00) >> 10
      val (sa, sr, da, dr) = getModes(memory(pc))
      val (newStatus, newHistory) =
        if (instruction <= 28 && checkFunctions(instruction)(sa, sr, da, dr, memory, pc))
          (MachineState.Run.Running, history)
        else
          (MachineState.Run.Aborted, invalidHistory)

      MachineState(memory, registers, pc, zf, cf, nf, newStatus, newHistory)
    }
  }


  private def singleStep(currentState: MachineState): MachineState = {
    require(currentState.status == MachineState.Run.Running, "single stepping a stopped machine")

    def getSourceOperand(
      memory   : Map[Int, Int],
      registers: Map[Int, Int],
      sa       : Int,
      sr       : Int,
      pc       : Int,
      history  : List[String]): (Int, Int, List[String]) = {

      sa match {
        case 0 =>
          // TODO: Check if the memory location is defined.
          (memory(pc + 1), pc + 1, history)

        case 1 =>
          // TODO: Check if the register is defined.
          (registers(sr), pc, history)

        case 2 =>
          // TODO: Check if the memory locations are defined.
          (memory(memory(pc + 1)), pc + 1, history)

        case 3 =>
          // TODO: Check if the register and the memory location are defined.
          (memory(registers(sr)), pc, history)

        case _ =>
          (0, pc, "Internal Error: Invalid addressing mode passed to getSourceOperand" :: history)
      }
    }

    // TODO: Implement me!
    def putDestinationOperand(
      operand  : Int,
      memory   : Map[Int, Int],
      registers: Map[Int, Int],
      da       : Int,
      dr       : Int,
      pc       : Int,
      history  : List[String]): (Map[Int, Int], Map[Int, Int], Int, List[String]) = {

      da match {
        case 0 =>
          (memory, registers, pc, "Internal Error: Invalid use of immediate addressing mode in putDestinationOperand" :: history)

        case 1 =>
          (memory, registers + (dr -> operand), pc, history)

        case 2 =>
          (memory + (memory(pc + 1) -> operand), registers, pc + 1, history)

        case 3 =>
          (memory + (registers(dr) -> operand), registers, pc, history)

        case _ =>
          (memory, registers, pc, "Internal Error: Invalid addressing mode passed to putDestinationOperand" :: history)
      }
    }

    val checkedState = checkCurrentOpcode(currentState)
    checkedState.status match {
      case MachineState.Run.Aborted =>
        checkedState

      case MachineState.Run.Running =>
        val MachineState(memory, registers, pc, zf, cf, nf, status, history) = checkedState
        val instruction = (memory(pc) & 0x7C00) >> 10
        val (sa, sr, da, dr) = getModes(memory(pc))

        instruction match {
          case 0 =>  // nop
            MachineState(memory, registers, pc + 1, zf, cf, nf, status, history)

          case 5 =>  // and
            val (src, pc1, history1) = getSourceOperand(memory, registers, sa, sr, pc,  history )
            val (dst, _,   history2) = getSourceOperand(memory, registers, da, dr, pc1, history1)
            val result = src & dst
            val (newMem, newReg, pc3, history3) =
              putDestinationOperand(result, memory, registers, da, dr, pc1, history2)
            MachineState(newMem, newReg, pc3 + 1, zf = result == 0, cf = false, nf, status, history3)

          case 8 =>  // inc
            val (src, pc1, history1) = getSourceOperand(memory, registers, da, dr, pc, history)
            val result = (src + 1) & 0x0000FFFF
            val (newMem, newReg, pc3, history3) =
              putDestinationOperand(result, memory, registers, da, dr, pc1, history1)
            MachineState(newMem, newReg, pc3 + 1, zf = result == 0, cf = result == 0, nf, status, history3)

          case 22 => // jc
            val (address, pc1, history1) = getSourceOperand(memory, registers, sa, sr, pc, history)
            if (cf)
              MachineState(memory, registers, address, zf, cf, nf, status, history1)
            else
              MachineState(memory, registers, pc1 + 1, zf, cf, nf, status, history1)

          case 24 => // stc
            MachineState(memory, registers, pc + 1, zf, cf = true, nf, status, history)

          case 25 => // clc
            MachineState(memory, registers, pc + 1, zf, cf = false, nf, status, history)

          case 28 => // halt
            MachineState(memory, registers, pc, zf, cf, nf, MachineState.Run.Halted, history)

          case _ =>
            val newHistory =
              "Unimplemented instruction %d at pc = %04X".format(instruction, pc) :: history
            MachineState(memory, registers, pc, zf, cf, nf, MachineState.Run.Aborted, newHistory)
        }
    }
  }


  /**
   * Runs a CPU simulation starting at the currentState. The simulation runs until the processor
   * halts or until it encounters an unrecoverable error.
   *
   * @param currentState The initial machine state for the simulation.
   * @return The final state when the simulation halts (if it does).
   */
  @tailrec
  def execute(currentState: MachineState): MachineState = {
    if (currentState.status != MachineState.Run.Running)
      currentState
    else
      execute(singleStep(currentState))
  }

}
