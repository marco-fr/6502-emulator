#include "cpu.hpp"

void CPU::execute(int num_cycles)
{
    int cycles = 0;
    while (cycles < num_cycles)
    {
        U8 opcode = read_byte(PC++);
        (this->*code[(int)opcode])((this->*addressing_mode[(int)opcode])());
        cycles += cycle_number[(int)opcode];
    }
}

void CPU::print_registers()
{
	std::cout << "Registers: " << std::endl;
    printf("A: %.2X ", A);
    printf("X: %.2X ", X);
    printf("Y: %.2X ", Y);
    printf("SP: %.2X ", SP);
    printf("PC: %.4X\n", PC);
	std::cout << std::endl;
}

void CPU::print_stack(){
	std::cout << "Stack: " << std::endl;
	for(int i = 0; i < 16; i++){
		for(int j = 0; j < 16; j++){
    		printf("%.2X ", read_byte(256 + i * 16 + j));
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void CPU::stack_push(U8 byte)
{
    write_byte(0x100 + SP, byte);
    if (SP == 0x00)
        SP = 0xFF;
    else
        SP--;
}

U8 CPU::stack_pop()
{
    if (SP == 0xFF)
        SP = 0x00;
    else
        SP++;
    return read_byte(0x00 + SP);
}

void CPU::write_byte(U16 position, U8 value) { mem->memory[position] = value; }

U8 CPU::read_byte(U16 position) { return mem->memory[position]; }

void CPU::write_word(U16 position, U16 value)
{
    U8 first_byte, second_byte;
    if (is_little_endian)
    {
        first_byte = value & 0xff;
        second_byte = ((value & ~0xff) >> 8);
    }
    else
    {
        second_byte = value & 0xff;
        first_byte = ((value & ~0xff) >> 8);
    }

    write_byte(position, first_byte);
    write_byte(position + 1, second_byte);
}

U16 CPU::read_word(U16 position)
{
    U8 first_byte = read_byte(position);
    U8 second_byte = read_byte(position + 1);
    if (is_little_endian)
        return (first_byte | (second_byte << 8));
    return (second_byte | (first_byte << 8));
}

void CPU::set_flag(int flag, int val)
{
    if (val)
        processor_status |= (1 << flag);
    else
        processor_status &= ~(1 << flag);
}

int CPU::get_flag(int flag) { return ((processor_status & (1 << flag)) != 0); }

void CPU::print_flags()
{
    std::cout << "Flags:" << std::endl;
	std::cout << "C  Z  I  D  B  -  V  N" << std::endl;
	for(int i = 0; i < 8; i++){
		std::cout << ((processor_status & (1 << i)) != 0) << "  ";
	}
    std::cout << std::endl << std::endl;
}

void CPU::reset()
{
    PC = 0x0600;
    A = 0, Y = 0, X = 0;
    SP = 0xFF;
    processor_status = 0x00;
}

void CPU::check_endian()
{
    union
    {
        uint32_t i;
        char c[4];
    } bint = {0x01020304};

    if (bint.c[0] == 1)
        is_little_endian = 0;
    else
        is_little_endian = 1;
    std::cout << "Little Endian: " << is_little_endian << std::endl;
}

void CPU::print_memory_byte(U16 position)
{
    std::cout << "Byte at address " << unsigned(position) << std::endl;
    print_byte(mem->memory[position]);
}

void CPU::print_byte(U8 byte)
{
    for (int i = 0; i < 8; i++)
    {
        if (byte & (1 << i))
            std::cout << "1 ";
        else
            std::cout << "0 ";
    }
    std::cout << std::endl;
}

void CPU::startup_info()
{
    std::cout << "MOS 6502 Processor Emulator" << std::endl;
}

CPU::CPU(Memory* memory)
{
    mem = memory;
    PC = 0;
    SP = 0;
    X = 0;
    Y = 0;
    processor_status = 0;
    startup_info();
    check_endian();
    reset();
    mem->load_bin_file();
}

U16 CPU::implied() { return 0; }

U16 CPU::accumulator() { return 0; }

U16 CPU::immediate() { return PC++; }

U16 CPU::absolute()
{
    U16 ea = read_word(PC);
    PC += 2;
    return ea;
}

U16 CPU::abs_x() { return absolute() + X; }

U16 CPU::abs_y() { return absolute() + Y; }

U16 CPU::inx()
{
    U16 r = (read_byte(PC++) + X) & 0xFF;
    return (read_byte(r + 1) << 8) + read_byte(r);
}

U16 CPU::iny()
{
    U16 r = (read_byte(PC++) + 1) & 0xFF;
    return (read_byte(r + 1) << 8) + read_byte(r) + Y;
}

U16 CPU::zero_page()
{
    U16 ea = read_byte(PC++);
    return ea;
}

U16 CPU::zero_x() { return (read_byte(PC++) + X) & 0xFF; }

U16 CPU::zero_y() { return (read_byte(PC++) + Y) & 0xFF; }

U16 CPU::abs_indirect()
{
    U16 LSB = read_word(PC++);
    U16 MSB = read_word(LSB + 1);
    return (MSB << 8) + LSB;
}

U16 CPU::relative()
{
    U16 r = (U16)read_byte(PC++);
    if (r & BIT_7_MASK)
        r |= 0xFF00;
    r += PC;
    return r;
}

U16 CPU::illegal_mode() { return 0; }

void CPU::OPCODE_ADC(U16 in)
{
    U16 m = read_byte(in);
    U8 is_carry = (get_flag(CARRY_FLAG) != 0);
    unsigned int total = (m + A + is_carry);
    set_flag(CARRY_FLAG, total > 0xFF);
    if (get_flag(DECIMAL_MODE))
    {
        if (total & 0x0F > 0x09)
            total += 0x06;
        set_flag(CARRY_FLAG, total > 0x99);
        if (total & 0xF0 > 0x90)
            total += 0x60;
    }
    U8 is_overflow = ((!((A ^ m) & BIT_7_MASK)) && ((A ^ total) & BIT_7_MASK));
    set_flag(OVERFLOW_FLAG, is_overflow);
    set_flag(NEGATIVE_FLAG, total & BIT_7_MASK);
    total &= 0xFF;
    set_flag(ZERO_FLAG, !total);

    A = total;
}

void CPU::OPCODE_AND(U16 in)
{
    // U8 m = A & mem->memory[in];
    U8 m = A & read_byte(in);
    set_flag(ZERO_FLAG, !m);
    set_flag(NEGATIVE_FLAG, m & BIT_7_MASK);
    A = m;
}

void CPU::OPCODE_ASL(U16 in)
{
    U8 m = read_byte(in);
    set_flag(CARRY_FLAG, m & BIT_7_MASK);
    m = (m << 1) & 0xFF;
    set_flag(ZERO_FLAG, !m);
    set_flag(NEGATIVE_FLAG, m & BIT_7_MASK);
    write_byte(in, m);
}

void CPU::OPCODE_ASL_ACC(U16 in)
{
    set_flag(CARRY_FLAG, A & BIT_7_MASK);
    A = (A << 1) & 0xFF;
    set_flag(ZERO_FLAG, !A);
    set_flag(NEGATIVE_FLAG, A & BIT_7_MASK);
}

void CPU::OPCODE_BCC(U16 in)
{
    if (!get_flag(CARRY_FLAG))
    {
        PC = in;
    }
}

void CPU::OPCODE_BCS(U16 in)
{
    if (get_flag(CARRY_FLAG))
    {
        PC = in;
    }
}

void CPU::OPCODE_BEQ(U16 in)
{
    if (get_flag(ZERO_FLAG))
    {
        PC = in;
    }
}

void CPU::OPCODE_BIT(U16 in)
{
    U8 r = read_byte(in);
    U8 m = A & r;
    set_flag(ZERO_FLAG, !m);
    set_flag(OVERFLOW_FLAG, r & BIT_6_MASK);
    set_flag(NEGATIVE_FLAG, r & BIT_7_MASK);
}

void CPU::OPCODE_BMI(U16 in)
{
    if (get_flag(NEGATIVE_FLAG))
    {
        PC = in;
    }
}

void CPU::OPCODE_BNE(U16 in)
{
    if (!get_flag(ZERO_FLAG))
    {
        PC = in;
    }
}

void CPU::OPCODE_BPL(U16 in)
{
    if (!get_flag(NEGATIVE_FLAG))
    {
        PC = in;
    }
}

void CPU::OPCODE_BRK(U16 in)
{
    PC++;
    set_flag(BREAK_COMMAND, 1);
    stack_push((PC >> 8) & 0xFF);
    stack_push(PC & 0xFF);
    stack_push(processor_status);
    PC = read_word(irqVector);
}

void CPU::OPCODE_BVC(U16 in)
{
    if (!get_flag(OVERFLOW_FLAG))
    {
        PC = in;
    }
}

void CPU::OPCODE_BVS(U16 in)
{
    if (get_flag(OVERFLOW_FLAG))
    {
        PC = in;
    }
}

void CPU::OPCODE_CLC(U16 in) { set_flag(CARRY_FLAG, 0); }

void CPU::OPCODE_CLD(U16 in) { set_flag(DECIMAL_MODE, 0); }

void CPU::OPCODE_CLI(U16 in) { set_flag(INTERRUPT_DISABLE, 0); }

void CPU::OPCODE_CLV(U16 in) { set_flag(OVERFLOW_FLAG, 0); }

void CPU::OPCODE_CMP(U16 in)
{
    U8 m = read_byte(in);
    set_flag(ZERO_FLAG, A == m);
    set_flag(CARRY_FLAG, A >= m);
    int result = A - m;
    set_flag(NEGATIVE_FLAG, result & BIT_7_MASK);
}

void CPU::OPCODE_CPX(U16 in)
{
    U8 m = read_byte(in);
    set_flag(ZERO_FLAG, X == m);
    set_flag(CARRY_FLAG, X >= m);
    int result = X - m;
    set_flag(NEGATIVE_FLAG, result & BIT_7_MASK);
}

void CPU::OPCODE_CPY(U16 in)
{
    U8 m = read_byte(in);
    set_flag(ZERO_FLAG, Y == m);
    set_flag(CARRY_FLAG, Y >= m);
    unsigned int result = Y - m;
    set_flag(NEGATIVE_FLAG, result & BIT_7_MASK);
}

void CPU::OPCODE_DEC(U16 in)
{
    U8 m = read_byte(in);
    m = (m - 1) & 0xFF;
    set_flag(ZERO_FLAG, !m);
    set_flag(NEGATIVE_FLAG, m & BIT_7_MASK);
    write_byte(in, m);
}

void CPU::OPCODE_DEX(U16 in)
{
    X = (X - 1) & 0xFF;
    set_flag(ZERO_FLAG, !X);
    set_flag(NEGATIVE_FLAG, X & BIT_7_MASK);
}

void CPU::OPCODE_DEY(U16 in)
{
    Y = (Y - 1) & 0xFF;
    set_flag(ZERO_FLAG, !Y);
    set_flag(NEGATIVE_FLAG, Y & BIT_7_MASK);
}

void CPU::OPCODE_EOR(U16 in)
{
    U8 m = read_byte(in);
    U8 result = m ^ A;
    set_flag(ZERO_FLAG, !result);
    set_flag(NEGATIVE_FLAG, result & BIT_7_MASK);
    A = result;
}

void CPU::OPCODE_INC(U16 in)
{
    U8 m = read_byte(in);
    m = (m + 1) & 0xFF;
    set_flag(ZERO_FLAG, !m);
    set_flag(NEGATIVE_FLAG, m & BIT_7_MASK);
    write_byte(in, m);
}

void CPU::OPCODE_INX(U16 in)
{
    X = (X + 1) & 0xFF;
    set_flag(ZERO_FLAG, !X);
    set_flag(NEGATIVE_FLAG, X & BIT_7_MASK);
}

void CPU::OPCODE_INY(U16 in)
{
    Y = (Y + 1) & 0xFF;
    set_flag(ZERO_FLAG, !Y);
    set_flag(NEGATIVE_FLAG, Y & BIT_7_MASK);
}

void CPU::OPCODE_JMP(U16 in) { PC = in; }

void CPU::OPCODE_JSR(U16 in)
{
    PC--;
    stack_push((PC >> 8) & 0xFF);
    stack_push(PC & 0xFF);
    PC = in;
}

void CPU::OPCODE_LDA(U16 in)
{
    U8 m = read_byte(in);
    set_flag(ZERO_FLAG, !m);
    set_flag(NEGATIVE_FLAG, m & BIT_7_MASK);

    A = m;
}

void CPU::OPCODE_LDX(U16 in)
{
    U8 m = read_byte(in);
    set_flag(ZERO_FLAG, !m);
    set_flag(NEGATIVE_FLAG, m & BIT_7_MASK);
    X = m;
}

void CPU::OPCODE_LDY(U16 in)
{
    U8 m = read_byte(in);
    set_flag(ZERO_FLAG, !m);
    set_flag(NEGATIVE_FLAG, m & BIT_7_MASK);
    Y = m;
}

void CPU::OPCODE_LSR(U16 in)
{
    U8 m = read_byte(in);
    set_flag(CARRY_FLAG, m & 0x01);
    m = (m >> 1) & 0xFF;
    set_flag(ZERO_FLAG, !m);
    set_flag(NEGATIVE_FLAG, m & BIT_7_MASK);
    write_byte(in, m);
}

void CPU::OPCODE_LSR_ACC(U16 in)
{
    set_flag(CARRY_FLAG, A & 0x01);
    A = (A >> 1) & 0xFF;
    set_flag(ZERO_FLAG, !A);
    set_flag(NEGATIVE_FLAG, A & BIT_7_MASK);
}

void CPU::OPCODE_NOP(U16 in)
{
    // PC++;
    return;
}

void CPU::OPCODE_ORA(U16 in)
{
    U8 m = read_byte(in);
    m |= A;
    set_flag(ZERO_FLAG, !m);
    set_flag(NEGATIVE_FLAG, m & BIT_7_MASK);
    A = m;
}

void CPU::OPCODE_PHA(U16 in) { stack_push(A); }

void CPU::OPCODE_PHP(U16 in) { stack_push(processor_status); }

void CPU::OPCODE_PLA(U16 in)
{
    A = stack_pop();
    set_flag(ZERO_FLAG, !A);
    set_flag(NEGATIVE_FLAG, A & BIT_7_MASK);
}

void CPU::OPCODE_PLP(U16 in)
{
    U8 m = stack_pop();
    processor_status = m;
}

void CPU::OPCODE_ROL(U16 in)
{
    U8 m = read_byte(in);
    U8 carry = m & BIT_7_MASK;
    m = (m << 1) & 0xFF;
    if (get_flag(CARRY_FLAG))
        m |= 0x01;
    set_flag(ZERO_FLAG, !m);
    set_flag(NEGATIVE_FLAG, m & BIT_7_MASK);
    set_flag(CARRY_FLAG, carry);
    write_byte(in, m);
}

void CPU::OPCODE_ROL_ACC(U16 in)
{
    U8 carry = A & BIT_7_MASK;
    A = (A << 1) & 0xFF;
    if (get_flag(CARRY_FLAG))
        A |= 0x01;
    set_flag(ZERO_FLAG, !A);
    set_flag(NEGATIVE_FLAG, A & BIT_7_MASK);
    set_flag(CARRY_FLAG, carry);
}

void CPU::OPCODE_ROR(U16 in)
{
    U8 m = read_byte(in);
    U8 carry = m & 0x01;
    m = (m >> 1) & 0xFF;
    if (get_flag(CARRY_FLAG))
        m |= BIT_7_MASK;
    set_flag(ZERO_FLAG, !m);
    set_flag(NEGATIVE_FLAG, m & BIT_7_MASK);
    set_flag(CARRY_FLAG, carry);
    write_byte(in, m);
}

void CPU::OPCODE_ROR_ACC(U16 in)
{
    U8 carry = A & 0x01;
    A = (A >> 1) & 0xFF;
    if (get_flag(CARRY_FLAG))
        A |= BIT_7_MASK;
    set_flag(ZERO_FLAG, !A);
    set_flag(NEGATIVE_FLAG, A & BIT_7_MASK);
    set_flag(CARRY_FLAG, carry);
}

void CPU::OPCODE_RTI(U16 in)
{
    U8 l, h;
    processor_status = stack_pop() | (1 << BREAK_COMMAND);
    l = stack_pop();
    h = stack_pop();
    PC = ((h << 8) | l);
}

void CPU::OPCODE_RTS(U16 in)
{
    U8 l, h;
    l = stack_pop();
    h = stack_pop();
    PC = ((h << 8) | l) + 1;
}

void CPU::OPCODE_SBC(U16 in)
{
    U8 m = read_byte(in);
    U8 carry = (get_flag(CARRY_FLAG) != 0);
    unsigned int result = (A - m - (1 - carry));
	U8 is_overflow = ((((A ^ m) & BIT_7_MASK)) && ((A ^ result) & BIT_7_MASK));
    set_flag(OVERFLOW_FLAG, is_overflow);
	result &= 0xFF;
    set_flag(ZERO_FLAG, !result);
    set_flag(NEGATIVE_FLAG, result & BIT_7_MASK);
}

void CPU::OPCODE_SEC(U16 in) { set_flag(CARRY_FLAG, 1); }

void CPU::OPCODE_SED(U16 in) { set_flag(DECIMAL_MODE, 1); }

void CPU::OPCODE_SEI(U16 in) { set_flag(INTERRUPT_DISABLE, 1); }

void CPU::OPCODE_STA(U16 in) { write_byte(in, A); }

void CPU::OPCODE_STX(U16 in) { write_byte(in, X); }
void CPU::OPCODE_STY(U16 in) { write_byte(in, Y); }

void CPU::OPCODE_TAX(U16 in)
{
    X = A;
    set_flag(ZERO_FLAG, !X);
    set_flag(NEGATIVE_FLAG, X & BIT_7_MASK);
}

void CPU::OPCODE_TAY(U16 in)
{
    Y = A;
    set_flag(ZERO_FLAG, !Y);
    set_flag(NEGATIVE_FLAG, Y & BIT_7_MASK);
}

void CPU::OPCODE_TSX(U16 in)
{
    X = SP;
    set_flag(ZERO_FLAG, !X);
    set_flag(NEGATIVE_FLAG, X & BIT_7_MASK);
}

void CPU::OPCODE_TXA(U16 in)
{
    A = X;
    set_flag(ZERO_FLAG, !X);
    set_flag(NEGATIVE_FLAG, X & BIT_7_MASK);
}

void CPU::OPCODE_TXS(U16 in)
{
    SP = X;
    set_flag(ZERO_FLAG, !X);
    set_flag(NEGATIVE_FLAG, X & BIT_7_MASK);
}

void CPU::OPCODE_TYA(U16 in)
{
    A = Y;
    set_flag(ZERO_FLAG, !Y);
    set_flag(NEGATIVE_FLAG, Y & BIT_7_MASK);
}

void CPU::OPCODE_ILLEGAL(U16 in)
{
    std::cout << "ILLEGAL OPCODE RUN" << std::endl;
    return;
}