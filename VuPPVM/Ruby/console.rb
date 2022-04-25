#!/usr/bin/ruby
###########################################################################
# FILE      : console.rb
# SUBJECT   : Console driver for VuPP simulation.
# PROGRAMMER: (C) Copyright 2012 by Peter C. Chapin
#
###########################################################################

require_relative "VuPPmemory"
require_relative "cpu"

# A single method to convert strings of digits to an integer.
def to_number(string)   
    if string =~ /^0[xX]/
        string.hex
    else
        string.to_i
    end
end

puts "VuPP Simulation Console v0.1"
puts "(C) Copyright 2012 by Peter Chapin"
puts ""

# Create the memory and the processor.
puts "Creating environment..."
$mem = VuPPmemoryDebug.new
$cpu = CPUDebug.new($mem)
$dump_address = 0
$dump_count = 32
puts "Done! Processor ready. Type 'help' for command list."

# To do: make it so that an empty command line repeats the last command.
while 1
    puts ""
    print "=> "
    args = gets.split(/[\s]+/)

    case args[0]
    when /^d/i
        $dump_address = to_number(args[1]) if args[1] != nil
        $dump_count = to_number(args[2]) if args[2] != nil
        $mem.dump($dump_address, $dump_count)
        $dump_address += $dump_count
        if $dump_address > 0xFFFF then $dump_address -= 0x10000 end

    when /^g/i
        if args[1] != nil then
            puts "[Not Implemented: Currently can't set PC in go command]"
        end
        while $cpu.step != 1 do
            print "PC = %04X\n" % [ $cpu.current_pc ]
        end
        puts "[Note: Processor executed halt instruction]"

    when /^l/i
        if args[1] == nil then
            puts "[Error: Name of object file required!]"
        else
            in_file = File.new(args[1], "r")
            pc = 0
            in_file.each_line { |line|
                if line =~ /^\.Version/i then
                    # Check version
                elsif line =~ /^\.Size/i then
                    # Check size (we'll assume it's 16 bits for now).
                elsif line =~ /^\.OJ/i then
                    fields = line.split(/\s+/)
                    fields.shift
                    fields.each { |field|
                        $mem.write(pc, field.hex)
                        pc += 1
                    }
                end
            }
            in_file.close
        end

    when /^h/i
        puts "dump [start [count]] -- Dumps memory"
        puts "go [start]           -- Runs processor at full speed"
        puts "load file.oj         -- Loads OJ file into memory"
        puts "poke address value   -- Writes 'value' to location 'address'"
        puts "quit                 -- Ends simulation"
        puts "reg                  -- Displays CPU registers"
        puts "step [start]         -- Executes single instruction"

    when /^p/i
        address = to_number(args[1])
        value = to_number(args[2])
        $mem.write(address, value)

    when /^q/i
        break

    when /^r/i
        $cpu.show_registers

    when /^s/i
        if args[1] != nil then
            puts "[Not Implemented: Currently can't set PC in step command]"
        end
        if $cpu.step == 1 then
            puts "[Note: Processor executed halt instruction]"
        end

    else
        puts "Unknown command: #{args[0]}"
    end
end
