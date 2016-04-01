/****************************************************************************
FILE      : node-types.cpp
SUBJECT   : Main program for vocal compiler.
PROGRAMMER: (C) Copyright 2003 by Peter Chapin

Please send comments or bug reports to

     Peter Chapin
     Vermont Technical College
     Williston, VT 05495
     PChapin@vtc.vsc.edu
****************************************************************************/

#include "node-types.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

using namespace std;

extern ofstream output;

// Declaring the symbol table.
extern std::map<std::string, symbol_attrs> symbol_table;


// This just returns a new label every time.
string next_label()
{
  static int labelnum = 0;
  // I'm generating the label in a C-style string and then returning it in a std::string.
  char label[10];
  sprintf(label, "_L%06d", labelnum++);
  string retval(label);
  return retval;
}

symbol_type and_node::generate()
{
  string label1 = next_label();
  string label2 = next_label();

  // Generate the left and right expressions first.
  symbol_type t_left  = left->generate();
  symbol_type t_right = right->generate();

  if (t_left == tERROR || t_right == tERROR) {
    return tERROR;
  }

  // Because we are expecting 0's and 1's on the stack from the relational nodes, having LONGs
  // there is an error.
  //
  if (t_left != tINT || t_right != tINT) {
    cerr << "ERROR - LONG type used in AND expression on line " 
	 << line_number << endl;
    return tERROR;
  }

  // Because I am planning on having all relational nodes leave either a 0 or 1 on the stack as
  // their result, I should be able to get away with using logical AND for my boolean AND.
  //
  output << "    pop r1" << endl
	 << "    pop r0" << endl
	 << "    and r1, r0" << endl
	 << "    jz " << label1 << endl
	 << "    push 1" << endl
	 << "    jmp " << label2 << endl
	 << label1 << ":" << endl
	 << "    push 0" << endl
	 << label2 << ":" << endl;
  return tINT;
}

symbol_type or_node::generate()
{
  string label1 = next_label();
  string label2 = next_label();

  // Generate the left and right expressions first.
  symbol_type t_left  = left->generate();
  symbol_type t_right = right->generate();

  if (t_left == tERROR || t_right == tERROR) {
    return tERROR;
  }

  // Because we are expecting 0's and 1's on the stack from the relational nodes, having LONGs
  // there is an error.
  //
  if (t_left != tINT || t_right != tINT) {
    cerr << "ERROR - LONG type used in AND expression on line " 
	 << line_number << endl;
    return tERROR;
  }

  // Because I am planning on having all relational nodes leave either a 0 or 1 on the stack as
  // their result, I should be able to get away with using logical OR for my boolean OR.
  //
  output << "    pop r1" << endl
	 << "    pop r0" << endl
	 << "    or r1, r0" << endl
	 << "    jz " << label1 << endl
	 << "    push 1" << endl
	 << "    jmp " << label2 << endl
	 << label1 << ":" << endl
	 << "    push 0" << endl
	 << label2 << ":" << endl;
  return tINT;
}

symbol_type relational_node::generate()
{
  string lblTrue = next_label();
  string lblFalse = next_label();
  string lblDone = next_label();
  string lblNext; 
  // Generate the left and right expressions first.
  symbol_type t_left  = left->generate();
  symbol_type t_right = right->generate();

  if (t_left == tERROR || t_right == tERROR) {
    return tERROR;
  }

  // What a mess this is. Convert the tINT to a tLONG for mismatched types.

  // Right is on top of the stack, so it's easiest to convert.
  if (t_right == tINT && t_left == tLONG) {
    // Take it off the stack and push a 0 as the MSW.
    output << "    pop r0" << endl
	   << "    push 0" << endl
	   << "    push r0" << endl;
    t_right = tLONG;
  }
  else if (t_left == tINT && t_right == tLONG) {
    // This is a little more complicated. The INT is underneath the LONG. Pop both off the stack
    // and then put a 0 in for the LSW of the INT.
    //
    output << "    pop r2" << endl
	   << "    pop r1" << endl
	   << "    pop r0" << endl
	   << "    push 0" << endl
	   << "    push r0" << endl
	   << "    push r1" << endl
	   << "    push r2" << endl;
    t_left = tLONG;
  }    


  switch (type) {
  case EQ_TYPE:
    if(t_left == tINT && t_right == tINT) {
      output << "    pop r1" << endl
	     << "    pop r0" << endl
	     << "    cmp r1, r0" << endl
	     << "    jnz " << lblFalse << endl
	     << "    push 1" << endl
	     << "    jmp " << lblDone << endl
	     << lblFalse << ":" << endl
	     << "    push 0" << endl
	     << lblDone << ":" << endl;
    }
    else if(t_left == tLONG && t_right == tLONG) {
      // Hmm... pop both longs off the stack (LSW first) and compare one word at a time.
      output << "    pop r0" << endl << "    pop r1" << endl
	     << "    pop r2" << endl << "    pop r3" << endl
	     << "    cmp r3, r1" << endl // Check the MSW first.
	     << "    jnz " << lblFalse << endl
	     << "    cmp r2, r0" << endl // Check the LSW next.
	     << "    jnz " << lblFalse << endl
	     << "    push 1" << endl // They were both equal!
	     << "    jmp " << lblDone << endl
	     << lblFalse << ":" << endl
	     << "    push 0" << endl // One of the words was not equal.
	     << lblDone << ":" << endl;
    }
    else {
      cerr << "Error in comparison between INT and LONG on line"
	   << line_number << endl;
    }
    break;

  case NE_TYPE:
    if(t_left == tINT && t_right == tINT) {
      output << "    pop r1" << endl
	     << "    pop r0" << endl
	     << "    cmp r1, r0" << endl
	     << "    jz " << lblFalse << endl
	     << "    push 1" << endl
	     << "    jmp " << lblDone << endl
	     << lblFalse << ":" << endl
	     << "    push 0" << endl
	     << lblDone << ":" << endl;
    }
    else if(t_left == tLONG && t_right == tLONG) {
      // Hmm...  pop both longs off the stack (LSW first) and compare
      // one word at a time.
      output << "    pop r0" << endl << "    pop r1" << endl
	     << "    pop r2" << endl << "    pop r3" << endl
	     << "    cmp r3, r1" << endl // Check the MSW first.
	     << "    jnz " << lblTrue << endl
	     << "    cmp r2, r0" << endl // Check the LSW next.
	     << "    jz " << lblFalse << endl
	     << lblTrue << ":" << endl
	     << "    push 1" << endl // They were both equal!
	     << "    jmp " << lblDone << endl
	     << lblFalse << ":" << endl
	     << "    push 0" << endl // One of the words was not equal.
	     << lblDone << ":" << endl;
    }
    else {
      cerr << "Error in comparison between INT and LONG on line"
	   << line_number << endl;
    }

    break;

  case LT_TYPE:
    if(t_left == tINT && t_right == tINT) {
      output << "    pop r1" << endl
	     << "    pop r0" << endl
	     << "    cmp r1, r0" << endl
	     << "    jz " << lblFalse << endl
	     << "    jnc " << lblFalse << endl
	     << "    push 1" << endl
	     << "    jmp " << lblDone << endl
	     << lblFalse << ":" << endl
	     << "    push 0" << endl
	     << lblDone << ":" << endl;
    }
    else if(t_left == tLONG && t_right == tLONG) {
      // We're going to need an extra label to make this work out right.
      lblNext = next_label();
      output << "    pop r3" << endl << "    pop r2" << endl
	     << "    pop r1" << endl << "    pop r0" << endl
	     << "    cmp r3, r1" << endl // Check the MSWs
	     << "    jz " << lblNext << endl // Equal - check LSWs
	     << "    jnc " << lblFalse << endl
	     << "    jmp " << lblTrue << endl
	     << lblNext << ":" << endl
	     << "    cmp r2, r0" << endl
	     << "    jz " << lblFalse << endl
	     << "    jnc " << lblFalse << endl
	     << lblTrue << ":" << endl
	     << "    push 1" << endl
	     << "    jmp " << lblDone << endl
	     << lblFalse << ":" << endl
	     << "    push 0" << endl
	     << lblDone << ":" << endl;
    }
    else {
      cerr << "Error in comparison between INT and LONG on line"
	   << line_number << endl;
    }

    break;

  case GT_TYPE:
    if(t_left == tINT && t_right == tINT) {
      output << "    pop r1" << endl
	     << "    pop r0" << endl
	     << "    cmp r0, r1" << endl
	     << "    jz " << lblFalse << endl
	     << "    jnc " << lblFalse << endl
	     << "    push 1" << endl
	     << "    jmp " << lblDone << endl
	     << lblFalse << ":" << endl
	     << "    push 0" << endl
	     << lblDone << ":" << endl;
    }
    else if(t_left == tLONG && t_right == tLONG) {
      // We're going to need an extra label to make this work out right.
      lblNext = next_label();
      output << "    pop r3" << endl << "    pop r2" << endl
	     << "    pop r1" << endl << "    pop r0" << endl
	     << "    cmp r1, r3" << endl // Check the MSWs
	     << "    jz " << lblNext << endl // Equal - check LSWs
	     << "    jnc " << lblFalse << endl
	     << "    jmp " << lblTrue << endl
	     << lblNext << ":" << endl
	     << "    cmp r0, r2" << endl
	     << "    jz " << lblFalse << endl
	     << "    jnc " << lblFalse << endl
	     << lblTrue << ":" << endl
	     << "    push 1" << endl
	     << "    jmp " << lblDone << endl
	     << lblFalse << ":" << endl
	     << "    push 0" << endl
	     << lblDone << ":" << endl;
    }
    else {
      cerr << "Error in comparison between INT and LONG on line"
	   << line_number << endl;
    }

    break;

    // LE is just the inverse of GT.
  case LE_TYPE:
    if(t_left == tINT && t_right == tINT) {
      output << "    pop r1" << endl
	     << "    pop r0" << endl
	     << "    cmp r1, r0" << endl
	     << "    jz " << lblTrue << endl
	     << "    jnc " << lblFalse << endl
	     << lblTrue << ":" << endl
	     << "    push 1" << endl
	     << "    jmp " << lblDone << endl
	     << lblFalse << ":" << endl
	     << "    push 0" << endl
	     << lblDone << ":" << endl;
    }
    else if(t_left == tLONG && t_right == tLONG) {
      // We're going to need an extra label to make this work out right.
      lblNext = next_label();
      output << "    pop r3" << endl << "    pop r2" << endl
	     << "    pop r1" << endl << "    pop r0" << endl
	     << "    cmp r3, r1" << endl // Check the MSWs
	     << "    jz " << lblNext << endl // Equal - check LSWs
	     << "    jnc " << lblFalse << endl
	     << "    jmp " << lblTrue << endl
	     << lblNext << ":" << endl
	     << "    cmp r2, r0" << endl
	     << "    jz " << lblTrue << endl
	     << "    jnc " << lblFalse << endl
	     << lblTrue << ":" << endl
	     << "    push 1" << endl
	     << "    jmp " << lblDone << endl
	     << lblFalse << ":" << endl
	     << "    push 0" << endl
	     << lblDone << ":" << endl;
    }
    else {
      cerr << "Error in comparison between INT and LONG on line"
	   << line_number << endl;
    }

    break;

    // GE is just the inverse of LT.
  case GE_TYPE:
    if(t_left == tINT && t_right == tINT) {
      output << "    pop r1" << endl
	     << "    pop r0" << endl
	     << "    cmp r0, r1" << endl
	     << "    jz " << lblTrue << endl
	     << "    jnc " << lblFalse << endl
	     << lblTrue << ":" << endl
	     << "    push 1" << endl
	     << "    jmp " << lblDone << endl
	     << lblFalse << ":" << endl
	     << "    push 0" << endl
	     << lblDone << ":" << endl;
    }
    else if(t_left == tLONG && t_right == tLONG) {
      // We're going to need an extra label to make this work out right.
      lblNext = next_label();
      output << "    pop r3" << endl << "    pop r2" << endl
	     << "    pop r1" << endl << "    pop r0" << endl
	     << "    cmp r1, r3" << endl // Check the MSWs
	     << "    jz " << lblNext << endl // Equal - check LSWs
	     << "    jnc " << lblFalse << endl
	     << "    jmp " << lblTrue << endl
	     << lblNext << ":" << endl
	     << "    cmp r0, r2" << endl
	     << "    jz " << lblTrue << endl
	     << "    jnc " << lblFalse << endl
	     << lblTrue << ":" << endl
	     << "    push 1" << endl
	     << "    jmp " << lblDone << endl
	     << lblFalse << ":" << endl
	     << "    push 0" << endl
	     << lblDone << ":" << endl;
    }
    else {
      cerr << "Error in comparison between INT and LONG on line"
	   << line_number << endl;
      return tERROR;
    }

    break;
  }
  return t_right;
}

symbol_type add_node::generate()
{
  // Generate the left and right expressions first.
  symbol_type t_left  = left->generate();
  symbol_type t_right = right->generate();

  if (t_left == tERROR || t_right == tERROR) {
    return tERROR;
  }
  // What a mess this is. Convert the tINT to a tLONG for mismatched types.

  // Right is on top of the stack, so it's easiest to convert.
  if (t_right == tINT && t_left == tLONG) {
    // Take it off the stack and push a 0 as the MSW.
    output << "    pop r0" << endl
	   << "    push 0" << endl
	   << "    push r0" << endl;
    t_right = tLONG;
  }
  else if (t_left == tINT && t_right == tLONG) {
    // This is a little more complicated. The INT is underneath the LONG. Pop both off the stack
    // and then put a 0 in for the LSW of the INT.
    //
    output << "    pop r2" << endl
	   << "    pop r1" << endl
	   << "    pop r0" << endl
	   << "    push 0" << endl
	   << "    push r0" << endl
	   << "    push r1" << endl
	   << "    push r2" << endl;
    t_left = tLONG;
  }    
  
  if(t_left == tINT && t_right == tINT) {
    output << "    pop r1" << endl
	   << "    pop r0" << endl
	   << "    clc" << endl 
	   << "    add r1, r0" << endl
	   << "    push r0" << endl;
    return tINT;
  }
  else if (t_left == tLONG && t_right == tLONG) {
    output << "    pop r0" << endl // LSW comes off stack first.
	   << "    pop r1" << endl
	   << "    pop r2" << endl
	   << "    pop r3" << endl
	   << "    clc" << endl 
	   << "    add r2, r0" << endl // Add without carry.
	   << "    add r3, r1" << endl // Add with carry.
	   << "    push r1" << endl // Push result MSW first.
	   << "    push r0" << endl;
    return tLONG;
  }
  else {
    cerr << "Error in addition between INT and LONG on line"
	 << line_number << endl;
    return tERROR;
  }
  
}

symbol_type sub_node::generate()
{
  // Generate the left and right expressions first.
  symbol_type t_left  = left->generate();
  symbol_type t_right = right->generate();

  if (t_left == tERROR || t_right == tERROR) {
    return tERROR;
  }
  // What a mess this is. Convert the tINT to a tLONG for mismatched types.

  // Right is on top of the stack, so it's easiest to convert.
  if (t_right == tINT && t_left == tLONG) {
    // Take it off the stack and push a 0 as the MSW.
    output << "    pop r0" << endl
	   << "    push 0" << endl
	   << "    push r0" << endl;
    t_right = tLONG;
  }
  else if (t_left == tINT && t_right == tLONG) {
    // This is a little more complicated. The INT is underneath the LONG. Pop both off the stack
    // and then put a 0 in for the LSW of the INT.
    //
    output << "    pop r2" << endl
	   << "    pop r1" << endl
	   << "    pop r0" << endl
	   << "    push 0" << endl
	   << "    push r0" << endl
	   << "    push r1" << endl
	   << "    push r2" << endl;
    t_left = tLONG;
  }    
  
  if(t_left == tINT && t_right == tINT) {
    output << "    pop r1" << endl
	   << "    pop r0" << endl
	   << "    clc" << endl 
	   << "    sub r1, r0" << endl
	   << "    push r0" << endl;
    return tINT;
  }
  else if (t_left == tLONG && t_right == tLONG) {
    output << "    pop r0" << endl // LSW comes off stack first.
	   << "    pop r1" << endl
	   << "    pop r2" << endl
	   << "    pop r3" << endl
	   << "    clc" << endl 
	   << "    sub r0, r2" << endl // Sub without carry.
	   << "    sub r1, r3" << endl // Sub with carry (borrow).
	   << "    push r3" << endl // Push result MSW first.
	   << "    push r2" << endl;
    return tLONG;
  }
  else {
    cerr << "Error in subtraction between INT and LONG on line"
	 << line_number << endl;
    return tERROR;
  }

}

symbol_type mul_node::generate()
{
  string label1 = next_label();
  string label2 = next_label();

  // Generate the left and right expressions first.
  symbol_type t_left  = left->generate();
  symbol_type t_right = right->generate();

  if (t_left != tINT || t_left != tINT) {
    cerr << "ERROR: Multiplication only supported with INTs on line"
	 << line_number << endl;
    return tERROR;
  }

  //  Yuck.

  // Use the registers to do repetitive addition.
  output << "    pop r0" << endl
	 << "    pop r1" << endl
	 << "    copy 0, r2" << endl
	 << label1 << ":" << endl
	 << "    cmp r0, 0" << endl
	 << "    jz " << label2 << endl
         << "    clc" << endl 
	 << "    add r1, r2" << endl
	 << "    dec r0" << endl
	 << "    jmp " << label1 << endl
	 << label2 << ":" << endl
         // Leave the result on the stack.
	 << "    push r2" << endl;
  return tINT;
}

symbol_type div_node::generate()
{
  string label1 = next_label();
  string label2 = next_label();

  // Generate the left and right expressions first.
  symbol_type t_left  = left->generate();
  symbol_type t_right = right->generate();

  if (t_left != tINT || t_left != tINT) {
    cerr << "ERROR: Multiplication only supported with INTs on line"
	 << line_number << endl;
    return tERROR;
  }

  //  Yuck.

  // Use the registers to do repetitive subtraction.
  output << "    pop r0" << endl        // Divisor
	 << "    pop r1" << endl 
	 << "    copy 0, r2" << endl     // Result
	 << label1 << ":" << endl
         << "    clc" << endl 
	 << "    sub r0, r1" << endl
	 << "    jc " << label2 << endl // Stop on a borrow.
	 << "    inc r2" << endl        // Count this subtraction.
	 << "    jmp " << label1 << endl
	 << label2 << ":" << endl
         // Leave the result on the stack.
	 << "    push r2" << endl;
  return tINT;
}

symbol_type num_node::generate()
{
  if (num_type == tINT) {
    output << "    push " << value << endl;
  }
  else if (num_type == tLONG) {
    // Put MSW first.
    output << "    push " << (value >> 16) << endl
	   << "    push " << (value & 0xFFFF) << endl;
  }
  else {
    cerr << " Can't generate number on line " << line_number << endl;
    return tERROR;
  }
  return num_type;
}


// This function places the contents of the variable on the stack and returns the type of the
// variable so the next node up knows how many words to remove from the stack.
//
symbol_type id_node::generate()
{
  // Look up the type of the symbol.
  std::map<std::string, symbol_attrs>::const_iterator symbol;
  symbol = symbol_table.find(name);
  if (symbol == symbol_table.end()) {
    cerr << "ERROR - symbol " << name << "on line " 
	 << line_number << " not found!" << endl;
    exit(1);
  }
  // If it's an int, just put it on the stack.
  if (symbol->second.vartype == tINT) {
    output << "    push (_" << name << ")" << endl;
    return tINT;
  }
  // If it's a long, more work needs to be done. I want to keep longs in memory on the stack in
  // the same byte ordering as normal memory, so I will need to push the higher address first
  // (because the stack grows toward lower addresses). Because the assembler is too dumb to be
  // able to do math for constants (eg. _varname + 1), I will have to use a register to do this.
  // This is inefficient at best, and the assembler should have constant math handling added.
  //
  if (symbol->second.vartype == tLONG) {
    
    output << "    copy _" << name << ", r0" << endl
	   << "    inc r0" << endl
	   << "    push (r0)" << endl
	   << "    dec r0" << endl
	   << "    push (r0)" << endl;
    return tLONG;
  }

}

symbol_type arrayref_node::generate()
{
  // Look up the type of the symbol.
  std::map<std::string, symbol_attrs>::const_iterator symbol;
  symbol = symbol_table.find(array_name);
  if (symbol == symbol_table.end()) {
    cerr << "ERROR - symbol " << array_name << " on line" 
	 << line_number << " not found!" << endl;
    return tERROR;
  }

  // Generating the index expression should leave the index on the top of the stack.
  symbol_type t_index = index_expression->generate();
  if (t_index == tERROR) {
    return tERROR;
  }

  // The index should only be an int. If it is a long, generate a warning and just use the LSW.
  if (t_index == tLONG) {
    cerr << "WARNING: LONG expression used as index into array " 
	 << array_name << " on line " << line_number << endl
	 << "Only lower word will be used." << endl;
    // Remove entire long (lower word first) and place just the lower word back on the stack.
    // This ensures the index on the stack is always one word long.
    output << "    pop r0" << endl
	   << "    pop r1" << endl
	   << "    push r0" << endl;
  }

  if (symbol->second.vartype == tINTARRAY) {
    // Use a register to manually locate the desired element. No bounds checking is done.
    output << "    pop r0" << endl
	   << "    copy _" << array_name << ", r1" << endl
	   << "    clc" << endl
	   << "    add r0, r1" << endl
	   << "    push (r1)" << endl;
    return tINT;
  }
  else if (symbol->second.vartype == tLONGARRAY) {
    // Use a register to manually locate the desired element. For a long, we need to multiply
    // the index by two (using a left shift). No bounds checking is done.
    output << "    pop r0" << endl
	   << "    clc" << endl    // Not sure if shift uses carry or not.
	   << "    shl r0" << endl
	   << "    copy _" << array_name << ", r1" << endl
	   << "    clc" << endl
	   << "    add r0, r1" << endl
	   << "    inc r1" << endl // Push the MSW first.
	   << "    push (r1)" << endl
	   << "    dec r1" << endl
	   << "    push (r1)" << endl;
    return tLONG;
  }

  // We should never get here.
  return tERROR;
}

void statementlist_node::generate()
{
  // Generate any previous statements.
  if (statement_list != NULL) {
    statement_list->generate();
  }
  // Generate this statement.
  statement->generate();
  return;
}

void if_node::generate()
{
  string label1 = next_label();
  string label2 = next_label();
  // Generate the expression, leaving the result on the top of the stack.
  expression->generate();
  // See what happened.
  output << "    pop r0" << endl
	 << "    cmp r0, 0" << endl
	 << "    jz " << label1 << endl;
  then_clause->generate();
  output << "    jmp " << label2 << endl
	 << label1 << ":" << endl;
  if(else_clause) else_clause->generate();
  output << label2 << ":" << endl;
  
  return;
}

void while_node::generate()
{
  string label1 = next_label();
  string label2 = next_label();

  // Check the expression each time at the top.
  output << label1 << ":" << endl;
  expression->generate();
  output << "    pop r0" << endl
	 << "    cmp r0, 0" << endl
	 << "    jz " << label2 << endl;
  statement_list->generate();
  output << "    jmp " << label1 << endl
	 << label2 << ":" << endl;
  return;
}

void return_node::generate()
{
  // Have the expression evaluate itself and just leave the result on the top of the stack.
  expression->generate();
  output << "    halt" << endl;
  return;
}

void assignment_node::generate()
{
  // Look up the type of the symbol.
  std::map<std::string, symbol_attrs>::const_iterator symbol;
  symbol = symbol_table.find(name);
  if (symbol == symbol_table.end()) {
    cerr << "ERROR - symbol " << name << " on line " 
	 << line_number << " not found!" << endl;
    return;
  }

  // Get the type of the generated expression.
  symbol_type t_expr = expression->generate();
  if (t_expr == tERROR) {
    cerr << "ERROR - error generating rvalue for assignment to " 
	 << name << " on line " << line_number << endl;
    return;
  }

  // Run through all of the possible lvalue/rvalue type combos.
  if (symbol->second.vartype == tINT) {
    if (t_expr == tINT) {
      output << "    pop (_" << name << ")" << endl;
    }
    else if (t_expr == tLONG) {
      cerr << "WARNING: LONG expression assigned to INT " 
	   << name << " on line " << line_number << endl
	   << "Only lower word will be used." << endl;
      // Lower word comes off stack first.
      output << "    pop (_" << name << ")" << endl
	     << "    pop r0" << endl; // Throw away the rest.
    }
  }
  else if (symbol->second.vartype == tLONG) {
    // Just move a 0 into the MSW.
    if (t_expr == tINT) {
      output << "    copy _" << name << ", r0" << endl
	     << "    pop (r0)" << endl
	     << "    inc r0" << endl
	     << "    copy 0, (r0)" << endl;
    }
    else if (t_expr == tLONG) {
      // Lower word comes off stack first.
      output << "    copy _" << name << ", r0" << endl
	     << "    pop (r0)" << endl
	     << "    inc r0" << endl
	     << "    pop (r0)" << endl;
    }
  }    
  return;
}

void arrayassign_node::generate()
{
  // Look up the type of the symbol.
  std::map<std::string, symbol_attrs>::const_iterator symbol;
  symbol = symbol_table.find(array_name);
  if (symbol == symbol_table.end()) {
    cerr << "ERROR - symbol " << array_name << " on line "
	 << line_number << " not found!" << endl;
    return;
  }

  // Get the type of the generated expression.
  symbol_type t_expr = expression->generate();
  if (t_expr == tERROR) {
    cerr << "ERROR - error generating rvalue for assignment to " 
	 << array_name << " on line " << line_number << endl;
    return;
  }

  // Leave the rvalue on the stack and generate the index.
  symbol_type t_index = index_expression->generate();
  if (t_index == tERROR) {
    cerr << "ERROR - unable to generate index expression for array " 
	 << array_name << " on line " << line_number << endl;
    return;
  }

  // The index should only be an int.  If it is a long, generate 
  // a warning and just use the LSW.
  if (t_index == tLONG) {
    cerr << "WARNING: LONG expression used as index into array " 
	 << array_name << " on line " << line_number << endl
	 << "Only lower word will be used." << endl;
    // Remove entire long (lower word first) and place just the lower word back on the stack.
    // This ensures the index on the stack is always one word long.
    output << "    pop r0" << endl
	   << "    pop r1" << endl
	   << "    push r0" << endl;
  }

  // Run through all of the possible lvalue/rvalue type combos.
  if (symbol->second.vartype == tINTARRAY) {
    if (t_expr == tINT) {
      // Compute the effective address.
      output << "    pop r0" << endl
	     << "    copy _" << array_name << ", r1" << endl
	     << "    add r0, r1" << endl
	     // Store the rvalue there.
	     << "    pop (r1)" << endl;
    }
    else if (t_expr == tLONG) {
      cerr << "WARNING: LONG expression assigned to INTARRAY " 
	   << array_name << " on line " << line_number << endl
	   << "Only lower word will be used." << endl;
      // Compute the effective address.
      output << "    pop r0" << endl
	     << "    copy _" << array_name << ", r1" << endl
	     << "    add r0, r1" << endl
	     // Store the rvalue there.
	     << "    pop (r1)" << endl
	     << "    pop r0" << endl; // Throw away the rest.
    }
  }
  else if (symbol->second.vartype == tLONGARRAY) {
    // Just move a 0 into the MSW.
    if (t_expr == tINT) {
      output << "    pop r0" << endl
	     << "    clc" << endl    // Not sure if shift uses carry or not.
	     << "    shl r0" << endl
	     << "    copy _" << array_name << ", r1" << endl
	     << "    clc" << endl
	     << "    add r0, r1" << endl
	     << "    pop (r1) " << endl // Pop of LSW first.
	     << "    inc r1" << endl
	     << "    copy 0, (r1)" << endl;
    }
    else if (t_expr == tLONG) {
      // Lower word comes off stack first. Use a register to manually locate the desired
      // element. For a long, we need to multiply the index by two (using a left shift). No
      // bounds checking is done.
      output << "    pop r0" << endl
	     << "    clc" << endl    // Not sure if shift uses carry or not.
	     << "    shl r0" << endl
	     << "    copy _" << array_name << ", r1" << endl
	     << "    clc" << endl
	     << "    add r0, r1" << endl
	     << "    pop (r1) " << endl // Pop of LSW first.
	     << "    inc r1" << endl
	     << "    pop (r1)" << endl;
    }
  }    
  return;

}
