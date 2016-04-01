###########################################################################
# FILE      : memory.rb
# SUBJECT   : Implementation of simulated memory.
# PROGRAMMER: (C) Copyright 2006 by Peter C. Chapin
#
# This file contains the code used to simulate memory in a virtual machine. To conserve space in
# the real machine this code stores only the memory fragments that are actually being used. If a
# read request comes in for a location that has not yet been initialized, it returns a special
# value (Should it throw an exception instead?)
#
# See the methods below for a "To Do" list. The way this code organizes the fragments is not
# very smart and could use significant improvement.
###########################################################################

class MemoryFragment

    def initialize(start)
        @start = start
        @data = ""
    end

    # To Do: Throw an exception if offset out of range?
    def read(offset)
        return @data[offset]
    end

    # To Do: Throw an exception if offset or value out of range?
    def write(offset, value)
        @data[offset] = value
    end

    def size
        @data.size
    end

    attr_reader :start

    # To Do: Handle fragment shrinkage properly.
    def resize(newsize, initializer)
        if newsize > @data.size
            count = newsize - @data.size
            count.times { @data.concat(initializer) }
        elsif newsize < @data.size
            # I don't know how to shrink the length of a string.
        end
    end
end

##########

# To Do: Implement a better way of storing the fragments. Use a tree?
class Memory

    # To Do: Throw an exception if the fragment size not a power of two?
    def initialize(initializer, fragment_size)
        @initializer = initializer
        @fragments = []
        @fragment_size = fragment_size
    end

    def read(address)
        @fragments.each { |frag|
            if address >= frag.start and address < (frag.start + frag.size)
                return frag.read(address - frag.start)
            end
        }
        return @initializer
    end

    def write(address, value)
        index, insert_slot = 0, 0
        @fragments.each { |frag|
            if address >= frag.start and address < (frag.start + frag.size)
                frag.write(address - frag.start, value)
                return
            end
            if address < frag.start
                insert_slot = index
            end
            index += 1
        }
        # If I get here, the address is not in any fragment.
        start_address = address & ~(@fragment_size - 1)
        @fragments[insert_slot, 0] = MemoryFragment.new(start_address)
        @fragments[insert_slot].resize(@fragment_size, @initializer);
        @fragments[insert_slot].write(address - start_address, value)
    end
end
