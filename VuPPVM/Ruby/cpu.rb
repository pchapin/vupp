###########################################################################
# FILE      : cpu.rb
# SUBJECT   : Implementation of VuPP CPU simulation.
# PROGRAMMER: (C) Copyright 2012 by Peter C. Chapin
#
###########################################################################

require_relative "VuPPmemory"

class CPU
    def initialize(mem)
        @mem = mem
        @reg = []
        8.times { @reg.push(0) }
        @pc = 0
        @carry_flag     = 0
        @zero_flag      = 0
        @interrupt_flag = 0
    end

    # Given an addressing mode and register, returns the operand.
    def get_source(mode, src_r)
        case mode
        when 0  # Immediate
            source = @mem.read(@pc)
            @pc += 1
        when 1  # Register direct
            source = @reg[src_r]
        when 2  # Indirect
            source_address = @mem.read(@pc)
            @pc += 1
            source = @mem.read(source_address)
        when 3  # Register indirect
            source_address = @reg[src_r]
            source = @mem.read(source_address)
        end
        return source
    end

    # Given an addressing mode, a register, and a code block...
    def put_dest(mode, dst_r)
        case mode
        when 1  # Register direct
            temp = @reg[dst_r]
            x = yield temp
            @reg[dst_r] = x
        when 2  # Indirect
            dst_address = @mem.read(@pc)
            @pc += 1
            temp = @mem.read(dst_address)
            new_value = yield temp
            @mem.write(dst_address, new_value)
        when 3  # Register indirect
            dst_address = @reg[dst_r]
            temp = @mem.read(dst_address)
            new_value = yield temp
            @mem.write(dst_address, new_value)
        end
    end

    # Sets the Z flag depending on the given argument.
    def condition_zflag(result)
        if result == 0
            @zero_flag = 1
        else
            @zero_flag = 0
        end
    end

    # Executes one instruction
    def step
        # Fetch the instruction.
        instruction = @mem.read(@pc)
        @pc += 1

        # Break the instruction into fields.
        dst_r    = instruction & 0x0007
        dst_mode = (instruction & 0x0018) >>  3
        src_r    = (instruction & 0x00E0) >>  5
        src_mode = (instruction & 0x0300) >>  8
        op_code  = (instruction & 0x7C00) >> 10

        # Execute the op code.
        case op_code
        when  1  # copy
            source = get_source(src_mode, src_r)
            put_dest(dst_mode, dst_r) { source }

        when  2  # add
            source = get_source(src_mode, src_r)
            put_dest(dst_mode, dst_r) { |current|
                result = current + source + @carry_flag
                if result > 0xFFFF
                    @carry_flag = 1
                    result &= 0xFFFF
                else
                    @carry_flag = 0
                end
                condition_zflag(result)
                result
            }

        when  3  # sub
            source = get_source(src_mode, src_r)
            put_dest(dst_mode, dst_r) { |current|
                result = current - source - @carry_flag
                if result < 0
                    @carry_flag = 1
                    result += 0x10000
                else
                    @carry_flag = 0
                end
                condition_zflag(result)
                result
            }

        when  4  # cmp
            source1 = get_source(src_mode, src_r)
            source2 = get_source(dst_mode, dst_r)
            if source2 == source1
                @zero_flag = 1
            else
                @zero_flag = 0
            end
            if source2 > source1
                @carry_flag = 0
            else
                @carry_flag = 1
            end

        when  5  # and
            source = get_source(src_mode, src_r)
            put_dest(dst_mode, dst_r) { |current|
                result = current & source
                condition_zflag(result)
                result
            }

        when  6  # or
            source = get_source(src_mode, src_r)
            put_dest(dst_mode, dst_r) { |current|
                result = current | source
                condition_zflag(result)
                result
            }

        when  7  # xor
            source = get_source(src_mode, src_r)
            put_dest(dst_mode, dst_r) { |current|
                result = current ^ source
                condition_zflag(result)
                result
            }

        when  8  # inc
            put_dest(dst_mode, dst_r) { |current|
                result = (current + 1) & 0xFFFF
                condition_zflag(result)
                result
            }

        when  9  # dec
            put_dest(dst_mode, dst_r) { |current|
                result = current - 1
                if result < 0
                    result += 0x10000
                end
                condition_zflag(result)
                result
            }

        when 10  # shl
            put_dest(dst_mode, dst_r) { |current|
                result = current << 1
                if result > 0xFFFF
                    @carry_flag = 1
                    result &= 0xFFFF
                else
                    @carry_flag = 0
                end
                condition_zflag(result)
                result
            }

        when 11  # shr
            put_dest(dst_mode, dst_r) { |current|
                if (current & 0x0001) == 1
                    @carry_flag = 1
                else
                    @carry_flag = 0
                end
                result = current >> 1
                condition_zflag(result)
                result
            }

        when 12  # rtl
            put_dest(dst_mode, dst_r) { |current|
                result = current << 1
                if result > 0xFFFF
                    @carry_flag = 1
                    result &= 0xFFFF
                    result |= 1
                else
                    @carry_flag = 0
                end
                condition_zflag(result)
                result
            }

        when 13  # rtr
            put_dest(dst_mode, dst_r) { |current|
                if (current & 0x0001) == 1
                    @carry_flag = 1
                else
                    @carry_flag = 0
                end
                result = current >> 1
                if @carry_flag == 1 then result |= 0x8000 end
                condition_zflag(result)
                result
            }

        when 14  # call
            source = get_source(src_mode, src_r)
            @reg[7] -= 1
            if @reg[7] < 0 then @reg[7] = 0xFFFF end
            @mem.write(@reg[7], @pc)
            @pc = source

        when 15  # ret
            result = @mem.read(@reg[7])
            @reg[7] += 1
            if @reg[7] > 0xFFFF then @reg[7] = 0 end
            @pc = result

        when 16  # reti
            # Not implemented.

        when 17  # push
            source = get_source(src_mode, src_r)
            @reg[7] -= 1
            if @reg[7] < 0 then @reg[7] = 0xFFFF end
            @mem.write(@reg[7], source)

        when 18  # pop
            put_dest(dst_mode, dst_r) { |current|
                result = @mem.read(@reg[7])
                @reg[7] += 1
                if @reg[7] > 0xFFFF then @reg[7] = 0 end
                result
            }

        when 19  # jmp
            source = get_source(src_mode, src_r)
            @pc = source

        when 20  # jz
            source = get_source(src_mode, src_r)
            if @zero_flag == 1
                @pc = source
            end

        when 21  # jnz
            source = get_source(src_mode, src_r)
            if @zero_flag == 0
                @pc = source
            end

        when 22  # jc
            source = get_source(src_mode, src_r)
            if @carry_flag == 1
                @pc = source
            end

        when 23  # jnc
            source = get_source(src_mode, src_r)
            if @carry_flag == 0
                @pc = source
            end

        when 24  # stc
            @carry_flag = 1

        when 25  # clc
            @carry_flag = 0

        when 26  # ei
            @interrupt_flag = 1

        when 27  # di
            @interrupt_flag = 0

        when 28  # halt
            return 1
        end
        return 0
    end

    # This runs the simulated CPU until it halts.
    def run
        while 1
            if step == 1 then break end
        end
    end
end


class CPUDebug < CPU

    def show_registers
        8.times { |i|
            print "r%d = %04X\n" % [i, @reg[i]]
        }
        print "pc = %04X, ZF = %i, CF = %i\n" % [ @pc, @zero_flag, @carry_flag ]
    end

    def current_pc
        return @pc
    end

end
