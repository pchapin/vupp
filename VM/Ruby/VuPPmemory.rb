#!/usr/bin/ruby
###########################################################################
# FILE          : VuPPmemory.rb
# SUBJECT       : Implementation of VuPP's simulated memory.
# PROGRAMMER    : (C) Copyright 2012 by Peter C. Chapin
#
# VuPP memory is 64K, 16 bit words. This implementation allocates all 128 KBytes at once. It
# uses the general purpose memory simulator but does not take advantage of the fragmentation
# handling provided by class Memory.
###########################################################################

require_relative "memory"

#
# VuPP memory contains 64K words. Each location is 16 bits.
#
class VuPPmemory < Memory

    # Fill the simulated memory with zeros to start.
    def initialize
        super(0x00, 2 * 0x10000)
    end

    # Return a 16 bit quantity.
    def read(address)
        (super(2*address) << 8) + super(2*address + 1)
    end

    # Write a 16 bit quantity as two 8 bit quantities.
    def write(address, value)
        super(2*address, (value & 0xFF00) >> 8)
        super(2*address + 1, value & 0x00FF)
    end
end


#
# This class implements console support methods.
#
class VuPPmemoryDebug < VuPPmemory

    # To do: Include ASCII decode (Unicode decode?) in the right margin.
    def dump(start, count)
        line_start, column = 1, 1
        while count > 0
            if line_start
                print "%04X | " % start
                line_start = nil
            end
            print "%04X  " % read(start)
            column += 1
            if column > 8
                print "\n"
                column = 1
                line_start = 1
            end
            count -= 1
            start += 1
            if start > 0xFFFF then start = 0 end
        end
    end

end
