#!/usr/bin/perl
###########################################################################
# FILE      : vas.pl
# SUBJECT   : Quick and dirty VuPP assembler.
# PROGRAMMER: (C) Copyright 2006 by Peter Chapin
# PROGRAMMER: Extra error handling by Samuel Colwell
#
# This program has minimal features.
#
# To Do:
#
# + Input lines should be detabified so that the list file prints okay.
#
# Please send comments or bug reports to
#
#      Peter C. Chapin
#      Computer Information Systems Department
#      Vermont Technical College
#      Williston, VT 05495
#      PChapin@vtc.vsc.edu
###########################################################################

#
# Accepts a string in either decimal or hex form and returns the value.
#
sub get_number {
    my($raw_number) = $_[0];
    if ($raw_number =~ /^0[xX]/) {
	return hex($raw_number);
    }
    # Support the 12ABh format
    if ($raw_number =~ /[0-9a-f]+h/i) {
	# Remove the h. (Why? --pchapin)
	$raw_number =~ s/h//;
	# Send this back as hex as well.
	return hex($raw_number);
    }
    return $raw_number;
}

#
# Accepts a string assumed to be an instruction and returns the operand code for that
# instructions. Unrecognized or invalid instructions return undef.
#
sub get_opcode {
    my($raw_string) = $_[0];

    # amodes contains regular expressions to match the various addressing modes.
    my(@amodes) = ("[0-9a-fx]+h?", "[rR][0-7]", "\\([0-9a-fx]+h?\\)", "\\([rR][0-7]\\)");
    my($all_modes) = "($amodes[0]|$amodes[1]|$amodes[2]|$amodes[3])";
    my($dest_modes) = "($amodes[1]|$amodes[2]|$amodes[3])";

    # Do not rearrange the order of the opcodes!
    my(@op_patterns) = (
	"NOP",
	"COPY\\s+$all_modes,\\s+$dest_modes",
	"ADD\\s+$all_modes,\\s+$dest_modes",
	"SUB\\s+$all_modes,\\s+$dest_modes",
	"CMP\\s+$all_modes,\\s+$all_modes", # Compare does not modify the second operand.
	"AND\\s+$all_modes,\\s+$dest_modes",
	"OR\\s+$all_modes,\\s+$dest_modes",
	"XOR\\s+$all_modes,\\s+$dest_modes",
	"INC\\s+$dest_modes", # These only take one operand, that they modify.
	"DEC\\s+$dest_modes",
	"SHL\\s+$dest_modes",
	"SHR\\s+$dest_modes",
	"RTL\\s+$dest_modes",
	"RTR\\s+$dest_modes",
	"CALL\\s+$all_modes",
	"RET",
	"RETI",
	"PUSH\\s+$all_modes",
	"POP\\s+$dest_modes",
	"JMP\\s+$all_modes",
	"JZ\\s+$all_modes",
	"JNZ\\s+$all_modes",
	"JC\\s+$all_modes",
	"JNC\\s+$all_modes",
	"STC",
	"CLC",
	"EI",
	"DI",
	"HALT");

    # Remove leading and trailing whitespace from the raw string.
    $raw_string =~ s/^\s+|\s+$//g;

    for ($index = 0; $index <= $#op_patterns; $index++) {
        if ($raw_string =~ /^$op_patterns[$index]$/i) { return $index; }
    }

    # This isn't going into the table of opcodes, but I do need to check for the
    # DW directive.
    if ($raw_string =~ /^DW\s+[0-9a-fx]+h?$/i) {
        return -1;  # This is not an opcode, but it's not undef either.
    }

    return undef;
}


#
# Accepts an operand string and a current pc value. Returns the new pc value. This subroutine is
# used when figuring out how many words an instruction consumes.
#
sub advance_pc {
    my($operand_string) = $_[0];
    my($current_pc) = $_[1];

    if ($operand_string =~ /^[rR][0-7]$/) {
        return $current_pc;
    }
    elsif ($operand_string =~ /^\([rR][0-7]\)$/) {
        return $current_pc;
    }
    elsif ($operand_string =~ /^\(.*\)$/) {
        return $current_pc + 1;
    }
    else {
        return $current_pc + 1;
    }
}

# ===========
# Main Script
# ===========

if ($#ARGV != 0) {
    print "Usage: vas filename.vas";
    exit(1);
}

$in_name = $ARGV[0];
$pc = 0;

# First pass... compute the address associated with each label.
open(INFILE, $in_name) || die "Can't open input file";
while (<INFILE>) {
    chomp;

    # Clean up the input a bit. (Ignore comments, blank lines, etc).
    s/;.*//;
    s/#.*//;
    s/\s+$//;
    if (/^\s*$/) { next; }

    @fields = split /[\s,]+/;

    # Check for assembler directives.
    if ($fields[1] =~ /ORG/i) {
        # The ORG statement must be indented.
        $pc = get_number($fields[2]);
        next;
    }
    if ($fields[1] =~ /EQU/i) {
	# Constants must start flush against the left margin.
        $constants{$fields[0]} = $fields[2];
        next;
    }

    # If the line starts with a label, remember it.
    if (/^[^\s]/) {
        $fields[0] =~ s/://;
        $labels{$fields[0]} = $pc;
    }

    if ($fields[1] =~ /DW/i) {
        # DW must be indented (but it can have a label).
        # It will take up exactly one word.
        $pc++;
        next;
    }

    shift(@fields);
    if ($#fields < 0) { next; }

    # If it's not an assembler directive, it's an instruction. Measure its size.
    $pc++;
    shift(@fields);
    if ($#fields >= 0) {
        $pc = advance_pc($fields[0], $pc);
    }
    shift(@fields);
    if ($#fields >= 0) {
        $pc = advance_pc($fields[0], $pc);
    }
}
close(INFILE);


$line_number = 0;
$pc = 0;
$out_name = $in_name;
$out_name =~ s/\.vas$//;
$out_name .= ".oj";

# Second pass... generate output
open(INFILE, $in_name) || die "Can't open input file";
open(OUTFILE, ">$out_name") || die "Can't open output file";
print OUTFILE ".Version 0.1\n";
print OUTFILE ".Size 16\n";

# Set a flag used to detect invalid instructions.
$invalid_instructions = 0;

while (<INFILE>) {
    $line_number++;
    chomp;
    $line_buffer = $_;

    # Clean up the input a bit. (Ignore comments, blank lines, etc).
    s/;.*//;
    s/#.*//;
    s/\s+$//;
    if (/^\s*$/) { print "     |              $line_buffer\n"; next; }

    $raw_string = $_;

    @fields = split /[\s,]+/, $raw_string;

    foreach $field (@fields) {
        # Replace any constants.
        $field =~ s/^\($_\)$/\($constants{$_}\)/g foreach (keys %constants);
        $field =~ s/^$_$/$constants{$_}/g foreach (keys %constants);

        # Replace any labels.
        $field =~ s/^\($_\)$/\($labels{$_}\)/g foreach (keys %labels);
        $field =~ s/^$_$/$labels{$_}/g foreach (keys %labels);
    }

    # Throw the first field away (label or empty).
    shift(@fields);
    if ($#fields < 0) { print "     |              $line_buffer\n"; next; }

    # Put modified fields back together for validation.
    $raw_string = "$fields[0] $fields[1]";
    $raw_string .= ", @fields[2..$#fields]" if @fields > 2;

    $start_pc = $pc;

    # Handle assembler directives.
    if ($fields[0] =~ /ORG/i) {
        $pc = get_number($fields[1]);
        printf "     |              $line_buffer\n";
        next;
    }
    # We've already handled the EQU constants, so ignore these for now.
    if ($fields[0] =~ /EQU/i) {
        printf "     |              $line_buffer\n";
        next;
    }


    # If it's not an assembler directive, it's an instruction.

    # Make sure it's a good unstruction.
    $op_code   = get_opcode($raw_string);
    if (!defined($op_code)) {
        # If it's no good, say so in the listing.
        printf "     | INVALID      $line_buffer\n";
        $invalid_instructions = 1;
        # Continue anyway.
        # WARNING: This will break all addresses after this point as the first pass counted this
	#          instruction and its operands to compute addresses.
        next;
    }

    $mode      =  0;
    $reg_num   =  0;
    $operand_1 = -1;
    $op_word   = $op_code << 10;
    $pc++;

    # Special check for the DW directive.
    if ($op_code == -1) {
        # In this case, the "opcode" to use is really the value the user wants.
        $op_word = get_number($fields[1]);
        # Empty out the rest of the fields so the rest of the assembler will
        # issue just the "opcode"
        @fields = ();
    }

    shift(@fields);
    if ($#fields >= 0) {

        # Decode addressing mode on first (or only) operand.
        if ($fields[0] =~ /^[rR]([0-7])$/) {
            $mode = 1;
            $reg_num = $1;
        }
        elsif ($fields[0] =~ /^\([rR]([0-7])\)$/) {
            $mode = 3;
            $reg_num = $1;
        }
        elsif ($fields[0] =~ /^\(.*\)$/) {
            $mode = 2;
            $pc++;
            $fields[0] =~ s/\(//;
            $fields[0] =~ s/\)//;
            if ($fields[0] =~ /^[0-9]/) { $operand_1 = get_number($fields[0]); }
            else { $operand_1 = $labels{$fields[0]}; }
        }
        else {
            $mode = 0;
            $pc++;
            if ($fields[0] =~ /^[0-9]/) { $operand_1 = get_number($fields[0]); }
            else { $operand_1 = $labels{$fields[0]}; }
        }

        # For certain op-codes, the first operand is the destination.
        if (($op_code >= 8 && $op_code <= 13) || $op_code == 18) {
            $op_word |= $mode << 3;
            if ($mode == 1 || $mode == 3) { $op_word |= $reg_num; }
        }
        # Otherwise the first operand is the source.
        else {
            $op_word |= $mode << 8;
            if ($mode == 1 || $mode == 3) { $op_word |= ($reg_num << 5); }
        }
    }

    $mode      =  0;
    $reg_num   =  0;
    $operand_2 = -1;
    shift(@fields);


    if ($#fields >= 0) {

        # Decode addressing mode on second operand.
        if ($fields[0] =~ /^[rR]([0-7])$/) {
            $mode = 1;
            $reg_num = $1;
        }
        elsif ($fields[0] =~ /^\([rR]([0-7])\)$/) {
            $mode = 3;
            $reg_num = $1;
        }
        elsif ($fields[0] =~ /^\(.*\)$/) {
            $mode = 2;
            $pc++;
            $fields[0] =~ s/\(//;
            $fields[0] =~ s/\)//;
            if ($fields[0] =~ /^[0-9]/) { $operand_2 = get_number($fields[0]); }
            else { $operand_2 = $labels{$fields[0]}; }
        }
        else {
            $mode = 0;
            $pc++;
            if ($fields[0] =~ /^[0-9]/) { $operand_2 = get_number($fields[0]); }
            else { $operand_2 = $labels{$fields[0]}; }
        }

        # In all cases the second operand (if it exists) is the destination.
        $op_word |= $mode << 3;
        if ($mode == 1 || $mode == 3) { $op_word |= $reg_num; }
    }

    # Print the resulting assembled code.
    if ($operand_1 == -1 and $operand_2 == -1) {
        printf "%04X | %04X         $line_buffer\n", $start_pc, $op_word;
        printf OUTFILE ".OJ %04X\n", $op_word;
    }
    elsif ($operand_1 == -1) {
        printf "%04X | %04X%04X     $line_buffer\n", $start_pc, $op_word, $operand_2;
        printf OUTFILE ".OJ %04X %04X\n", $op_word, $operand_2;
    }
    elsif ($operand_2 == -1) {
        printf "%04X | %04X%04X     $line_buffer\n", $start_pc, $op_word, $operand_1;
        printf OUTFILE ".OJ %04X %04X\n", $op_word, $operand_1;
    }
    else {
        printf "%04X | %04X%04X%04X $line_buffer\n", $start_pc, $op_word, $operand_1, $operand_2;
        printf OUTFILE ".OJ %04X %04X %04X\n", $op_word, $operand_1, $operand_2;
    }
}
close(OUTFILE);
close(INFILE);

# Check to see if we found any invalid instructions.
if ($invalid_instructions) {
    print STDERR "There were invalid instructions.\n";
    print STDERR "The generated output will not be valid.\n";
}
