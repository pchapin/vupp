
The VTC Microprocessor Project (VuPP)
=====================================

This project is a rather grand effort to design and eventually implement a simple microprocessor
from the gate level (more realistically using a hardware description language). Currently, the
only components of the system are software oriented as described briefly below.

The primary language of implementation is Scala using the SBT build tool. However, some of the
tools have previous (incomplete) implementations in other languages mentioned here.

+ Fink. The linker for the VuPP processor (formerly in C++). The object file format used is a
  custom format for this project. As a future enhancement it might make sense to support one of
  the standard formats.

+ VAS. The assembler for the VuPP processor (formerly in Perl). This project is complete enough
  to be useful, allowing VuPP programs to actuall be created.
  
+ VM. The virtual machine simulator for the VuPP processor (formerly in Ruby). This project
  allows assembled VuPP programs to be "executed" on a virtual machine running on Windows or
  Linux. This can be used to test VuPP software before any VuPP hardware exists.
  
+ Vocal. The compiler for the vocal language (formerly in C++). Vocal is a simplified Ada-like
  language. See also the [Augusta](https://github.com/pchapin/augusta) project for a compiler
  which compiles a more complex Ada-like language into LLVM assembly language. A VuPP code
  generator for LLVM might be a nice addition to this repository.

The www folder contains documentation for the project.

Peter C. Chapin  
chapinp@acm.org  
