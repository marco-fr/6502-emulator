#include "memory.hpp"
#include <iostream>

#define U8 uint8_t
#define U16 uint16_t
#define U32 uint32_t

enum
{
    CARRY_FLAG,
    ZERO_FLAG,
    INTERRUPT_DISABLE,
    DECIMAL_MODE,
    BREAK_COMMAND,
    UNUSED,
    OVERFLOW_FLAG,
    NEGATIVE_FLAG
};

class CPU
{
public:
    U8 read_byte(U16 position);
    void write_byte(U16 position, U8 value);

    U16 read_word(U16 position);
    void write_word(U16 position, U16 value);
    void check_endian();
    void startup_info();
    void print_byte(U8 byte);
    void print_memory_byte(U16 position);
    void print_flags();
    void print_registers();
    void print_stack();
    void execute(int num_cycles);
    void reset();

    CPU(Memory* memory);

private:
    void set_flag(int flag, int val);
    int get_flag(int flag);

    void stack_push(U8 byte);
    U8 stack_pop();

    // Opcodes
    void OPCODE_ADC(U16 in); // idk
    void OPCODE_AND(U16 in);
    void OPCODE_ASL(U16 in);
    void OPCODE_ASL_ACC(U16 in);
    void OPCODE_BCC(U16 in);
    void OPCODE_BCS(U16 in);
    void OPCODE_BEQ(U16 in);
    void OPCODE_BIT(U16 in);
    void OPCODE_BMI(U16 in);
    void OPCODE_BNE(U16 in);
    void OPCODE_BPL(U16 in);
    void OPCODE_BRK(U16 in);
    void OPCODE_BVC(U16 in);
    void OPCODE_BVS(U16 in);
    void OPCODE_CLC(U16 in);
    void OPCODE_CLD(U16 in);
    void OPCODE_CLI(U16 in);
    void OPCODE_CLV(U16 in);
    void OPCODE_CMP(U16 in);
    void OPCODE_CPX(U16 in);
    void OPCODE_CPY(U16 in);
    void OPCODE_DEC(U16 in);
    void OPCODE_DEX(U16 in);
    void OPCODE_DEY(U16 in);
    void OPCODE_EOR(U16 in);
    void OPCODE_INC(U16 in);
    void OPCODE_INX(U16 in);
    void OPCODE_INY(U16 in);
    void OPCODE_JMP(U16 in);
    void OPCODE_JSR(U16 in);
    void OPCODE_LDA(U16 in);
    void OPCODE_LDX(U16 in);
    void OPCODE_LDY(U16 in);
    void OPCODE_LSR(U16 in);
    void OPCODE_LSR_ACC(U16 in);
    void OPCODE_NOP(U16 in);
    void OPCODE_ORA(U16 in);
    void OPCODE_PHA(U16 in);
    void OPCODE_PHP(U16 in);
    void OPCODE_PLA(U16 in);
    void OPCODE_PLP(U16 in);
    void OPCODE_ROL(U16 in);
    void OPCODE_ROL_ACC(U16 in);
    void OPCODE_ROR(U16 in);
    void OPCODE_ROR_ACC(U16 in);
    void OPCODE_RTI(U16 in);
    void OPCODE_RTS(U16 in);
    void OPCODE_SBC(U16 in); // figure out
    void OPCODE_SEC(U16 in);
    void OPCODE_SED(U16 in);
    void OPCODE_SEI(U16 in);
    void OPCODE_STA(U16 in);
    void OPCODE_STX(U16 in);
    void OPCODE_STY(U16 in);
    void OPCODE_TAX(U16 in);
    void OPCODE_TAY(U16 in);
    void OPCODE_TSX(U16 in);
    void OPCODE_TXA(U16 in);
    void OPCODE_TXS(U16 in);
    void OPCODE_TYA(U16 in);
    void OPCODE_ILLEGAL(U16 in);

    const U8 BIT_7_MASK = 0x80;
    const U8 BIT_6_MASK = 0x40;

    const U16 irqVector = 0xFFFE;
    const U16 nmiVector = 0xFFFA;
    const U16 resetVector = 0xFFC;

    int is_little_endian = 0;
    U16 PC; // Program Counter
    U8 SP;  // Stack pointer
    U8 A;   // Accumulator
    U8 X;   // Index Register X
    U8 Y;   // Index Register Y
    U8 processor_status = 0;
    Memory* mem;

    // Addressing modes
    U16 implied();
    U16 accumulator();
    U16 immediate();
    U16 absolute();
    U16 zero_page();
    U16 abs_x();
    U16 abs_y();
    U16 zero_x();
    U16 zero_y();
    U16 abs_indirect();
    U16 inx();
    U16 iny();
    U16 relative();
    U16 illegal_mode();

    // OPCODE numbers taken from https://www.pagetable.com/c64ref/6502/?tab=3
    void (CPU::*code[256])(U16){
        &CPU::OPCODE_BRK,     &CPU::OPCODE_ORA,     &CPU::OPCODE_ILLEGAL,
        &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_ORA,
        &CPU::OPCODE_ASL,     &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_PHP,
        &CPU::OPCODE_ORA,     &CPU::OPCODE_ASL_ACC, &CPU::OPCODE_ILLEGAL,
        &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_ORA,     &CPU::OPCODE_ASL,
        &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_BPL,     &CPU::OPCODE_ORA,
        &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_ILLEGAL,
        &CPU::OPCODE_ORA,     &CPU::OPCODE_ASL,     &CPU::OPCODE_ILLEGAL,
        &CPU::OPCODE_CLC,     &CPU::OPCODE_ORA,     &CPU::OPCODE_ILLEGAL,
        &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_ORA,
        &CPU::OPCODE_ASL,     &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_JSR,
        &CPU::OPCODE_AND,     &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_ILLEGAL,
        &CPU::OPCODE_BIT,     &CPU::OPCODE_AND,     &CPU::OPCODE_ROL,
        &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_PLP,     &CPU::OPCODE_AND,
        &CPU::OPCODE_ROL_ACC, &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_BIT,
        &CPU::OPCODE_AND,     &CPU::OPCODE_ROL,     &CPU::OPCODE_ILLEGAL,
        &CPU::OPCODE_BMI,     &CPU::OPCODE_AND,     &CPU::OPCODE_ILLEGAL,
        &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_AND,
        &CPU::OPCODE_ROL,     &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_SEC,
        &CPU::OPCODE_AND,     &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_ILLEGAL,
        &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_AND,     &CPU::OPCODE_ROL,
        &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_RTI,     &CPU::OPCODE_EOR,
        &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_ILLEGAL,
        &CPU::OPCODE_EOR,     &CPU::OPCODE_LSR,     &CPU::OPCODE_ILLEGAL,
        &CPU::OPCODE_PHA,     &CPU::OPCODE_EOR,     &CPU::OPCODE_LSR_ACC,
        &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_JMP,     &CPU::OPCODE_EOR,
        &CPU::OPCODE_LSR,     &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_BVC,
        &CPU::OPCODE_EOR,     &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_ILLEGAL,
        &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_EOR,     &CPU::OPCODE_LSR,
        &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_CLI,     &CPU::OPCODE_EOR,
        &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_ILLEGAL,
        &CPU::OPCODE_EOR,     &CPU::OPCODE_LSR,     &CPU::OPCODE_ILLEGAL,
        &CPU::OPCODE_RTS,     &CPU::OPCODE_ADC,     &CPU::OPCODE_ILLEGAL,
        &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_ADC,
        &CPU::OPCODE_ROR,     &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_PLA,
        &CPU::OPCODE_ADC,     &CPU::OPCODE_ROR_ACC, &CPU::OPCODE_ILLEGAL,
        &CPU::OPCODE_JMP,     &CPU::OPCODE_ADC,     &CPU::OPCODE_ROR,
        &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_BVS,     &CPU::OPCODE_ADC,
        &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_ILLEGAL,
        &CPU::OPCODE_ADC,     &CPU::OPCODE_ROR,     &CPU::OPCODE_ILLEGAL,
        &CPU::OPCODE_SEI,     &CPU::OPCODE_ADC,     &CPU::OPCODE_ILLEGAL,
        &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_ADC,
        &CPU::OPCODE_ROR,     &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_ILLEGAL,
        &CPU::OPCODE_STA,     &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_ILLEGAL,
        &CPU::OPCODE_STY,     &CPU::OPCODE_STA,     &CPU::OPCODE_STX,
        &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_DEY,     &CPU::OPCODE_ILLEGAL,
        &CPU::OPCODE_TXA,     &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_STY,
        &CPU::OPCODE_STA,     &CPU::OPCODE_STX,     &CPU::OPCODE_ILLEGAL,
        &CPU::OPCODE_BCC,     &CPU::OPCODE_STA,     &CPU::OPCODE_ILLEGAL,
        &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_STY,     &CPU::OPCODE_STA,
        &CPU::OPCODE_STX,     &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_TYA,
        &CPU::OPCODE_STA,     &CPU::OPCODE_TXS,     &CPU::OPCODE_ILLEGAL,
        &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_STA,     &CPU::OPCODE_ILLEGAL,
        &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_LDY,     &CPU::OPCODE_LDA,
        &CPU::OPCODE_LDX,     &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_LDY,
        &CPU::OPCODE_LDA,     &CPU::OPCODE_LDX,     &CPU::OPCODE_ILLEGAL,
        &CPU::OPCODE_TAY,     &CPU::OPCODE_LDA,     &CPU::OPCODE_TAX,
        &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_LDY,     &CPU::OPCODE_LDA,
        &CPU::OPCODE_LDX,     &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_BCS,
        &CPU::OPCODE_LDA,     &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_ILLEGAL,
        &CPU::OPCODE_LDY,     &CPU::OPCODE_LDA,     &CPU::OPCODE_LDX,
        &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_CLV,     &CPU::OPCODE_LDA,
        &CPU::OPCODE_TSX,     &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_LDY,
        &CPU::OPCODE_LDA,     &CPU::OPCODE_LDX,     &CPU::OPCODE_ILLEGAL,
        &CPU::OPCODE_CPY,     &CPU::OPCODE_CMP,     &CPU::OPCODE_ILLEGAL,
        &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_CPY,     &CPU::OPCODE_CMP,
        &CPU::OPCODE_DEC,     &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_INY,
        &CPU::OPCODE_CMP,     &CPU::OPCODE_DEX,     &CPU::OPCODE_ILLEGAL,
        &CPU::OPCODE_CPY,     &CPU::OPCODE_CMP,     &CPU::OPCODE_DEC,
        &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_BNE,     &CPU::OPCODE_CMP,
        &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_ILLEGAL,
        &CPU::OPCODE_CMP,     &CPU::OPCODE_DEC,     &CPU::OPCODE_ILLEGAL,
        &CPU::OPCODE_CLD,     &CPU::OPCODE_CMP,     &CPU::OPCODE_ILLEGAL,
        &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_CMP,
        &CPU::OPCODE_DEC,     &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_CPX,
        &CPU::OPCODE_SBC,     &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_ILLEGAL,
        &CPU::OPCODE_CPX,     &CPU::OPCODE_SBC,     &CPU::OPCODE_INC,
        &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_INX,     &CPU::OPCODE_SBC,
        &CPU::OPCODE_NOP,     &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_CPX,
        &CPU::OPCODE_SBC,     &CPU::OPCODE_INC,     &CPU::OPCODE_ILLEGAL,
        &CPU::OPCODE_BEQ,     &CPU::OPCODE_SBC,     &CPU::OPCODE_ILLEGAL,
        &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_SBC,
        &CPU::OPCODE_INC,     &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_SED,
        &CPU::OPCODE_SBC,     &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_ILLEGAL,
        &CPU::OPCODE_ILLEGAL, &CPU::OPCODE_SBC,     &CPU::OPCODE_INC,
        &CPU::OPCODE_ILLEGAL};

    U16(CPU::*addressing_mode[256])
    (void) = {&CPU::implied,      &CPU::inx,          &CPU::illegal_mode,
              &CPU::illegal_mode, &CPU::illegal_mode, &CPU::zero_page,
              &CPU::zero_page,    &CPU::illegal_mode, &CPU::implied,
              &CPU::immediate,    &CPU::accumulator,  &CPU::illegal_mode,
              &CPU::illegal_mode, &CPU::absolute,     &CPU::absolute,
              &CPU::illegal_mode, &CPU::relative,     &CPU::iny,
              &CPU::illegal_mode, &CPU::illegal_mode, &CPU::illegal_mode,
              &CPU::zero_x,       &CPU::zero_x,       &CPU::illegal_mode,
              &CPU::implied,      &CPU::abs_y,        &CPU::illegal_mode,
              &CPU::illegal_mode, &CPU::illegal_mode, &CPU::abs_x,
              &CPU::abs_x,        &CPU::illegal_mode, &CPU::absolute,
              &CPU::inx,          &CPU::illegal_mode, &CPU::illegal_mode,
              &CPU::zero_page,    &CPU::zero_page,    &CPU::zero_page,
              &CPU::illegal_mode, &CPU::implied,      &CPU::immediate,
              &CPU::accumulator,  &CPU::illegal_mode, &CPU::absolute,
              &CPU::absolute,     &CPU::absolute,     &CPU::illegal_mode,
              &CPU::relative,     &CPU::iny,          &CPU::illegal_mode,
              &CPU::illegal_mode, &CPU::illegal_mode, &CPU::zero_x,
              &CPU::zero_x,       &CPU::illegal_mode, &CPU::implied,
              &CPU::abs_y,        &CPU::illegal_mode, &CPU::illegal_mode,
              &CPU::illegal_mode, &CPU::abs_x,        &CPU::abs_x,
              &CPU::illegal_mode, &CPU::implied,      &CPU::inx,
              &CPU::illegal_mode, &CPU::illegal_mode, &CPU::illegal_mode,
              &CPU::zero_page,    &CPU::zero_page,    &CPU::illegal_mode,
              &CPU::implied,      &CPU::immediate,    &CPU::accumulator,
              &CPU::illegal_mode, &CPU::absolute,     &CPU::absolute,
              &CPU::absolute,     &CPU::illegal_mode, &CPU::relative,
              &CPU::iny,          &CPU::illegal_mode, &CPU::illegal_mode,
              &CPU::illegal_mode, &CPU::zero_x,       &CPU::zero_x,
              &CPU::illegal_mode, &CPU::implied,      &CPU::abs_y,
              &CPU::illegal_mode, &CPU::illegal_mode, &CPU::illegal_mode,
              &CPU::abs_x,        &CPU::abs_x,        &CPU::illegal_mode,
              &CPU::implied,      &CPU::inx,          &CPU::illegal_mode,
              &CPU::illegal_mode, &CPU::illegal_mode, &CPU::zero_page,
              &CPU::zero_page,    &CPU::illegal_mode, &CPU::implied,
              &CPU::immediate,    &CPU::accumulator,  &CPU::illegal_mode,
              &CPU::abs_indirect, &CPU::absolute,     &CPU::absolute,
              &CPU::illegal_mode, &CPU::relative,     &CPU::iny,
              &CPU::illegal_mode, &CPU::illegal_mode, &CPU::illegal_mode,
              &CPU::zero_x,       &CPU::zero_x,       &CPU::illegal_mode,
              &CPU::implied,      &CPU::abs_y,        &CPU::illegal_mode,
              &CPU::illegal_mode, &CPU::illegal_mode, &CPU::abs_x,
              &CPU::abs_x,        &CPU::illegal_mode, &CPU::illegal_mode,
              &CPU::inx,          &CPU::illegal_mode, &CPU::illegal_mode,
              &CPU::zero_page,    &CPU::zero_page,    &CPU::zero_page,
              &CPU::illegal_mode, &CPU::implied,      &CPU::illegal_mode,
              &CPU::implied,      &CPU::illegal_mode, &CPU::absolute,
              &CPU::absolute,     &CPU::absolute,     &CPU::illegal_mode,
              &CPU::relative,     &CPU::iny,          &CPU::illegal_mode,
              &CPU::illegal_mode, &CPU::zero_x,       &CPU::zero_x,
              &CPU::zero_y,       &CPU::illegal_mode, &CPU::implied,
              &CPU::abs_y,        &CPU::implied,      &CPU::illegal_mode,
              &CPU::illegal_mode, &CPU::abs_x,        &CPU::illegal_mode,
              &CPU::illegal_mode, &CPU::immediate,    &CPU::inx,
              &CPU::immediate,    &CPU::illegal_mode, &CPU::zero_page,
              &CPU::zero_page,    &CPU::zero_page,    &CPU::illegal_mode,
              &CPU::implied,      &CPU::immediate,    &CPU::implied,
              &CPU::illegal_mode, &CPU::absolute,     &CPU::absolute,
              &CPU::absolute,     &CPU::illegal_mode, &CPU::relative,
              &CPU::iny,          &CPU::illegal_mode, &CPU::illegal_mode,
              &CPU::zero_x,       &CPU::zero_x,       &CPU::zero_y,
              &CPU::illegal_mode, &CPU::implied,      &CPU::abs_y,
              &CPU::implied,      &CPU::illegal_mode, &CPU::abs_x,
              &CPU::abs_x,        &CPU::abs_y,        &CPU::illegal_mode,
              &CPU::immediate,    &CPU::inx,          &CPU::illegal_mode,
              &CPU::illegal_mode, &CPU::zero_page,    &CPU::zero_page,
              &CPU::zero_page,    &CPU::illegal_mode, &CPU::implied,
              &CPU::immediate,    &CPU::implied,      &CPU::illegal_mode,
              &CPU::absolute,     &CPU::absolute,     &CPU::absolute,
              &CPU::illegal_mode, &CPU::relative,     &CPU::iny,
              &CPU::illegal_mode, &CPU::illegal_mode, &CPU::illegal_mode,
              &CPU::zero_x,       &CPU::zero_x,       &CPU::illegal_mode,
              &CPU::implied,      &CPU::abs_y,        &CPU::illegal_mode,
              &CPU::illegal_mode, &CPU::illegal_mode, &CPU::abs_x,
              &CPU::abs_x,        &CPU::illegal_mode, &CPU::immediate,
              &CPU::inx,          &CPU::illegal_mode, &CPU::illegal_mode,
              &CPU::zero_page,    &CPU::zero_page,    &CPU::zero_page,
              &CPU::illegal_mode, &CPU::implied,      &CPU::immediate,
              &CPU::implied,      &CPU::illegal_mode, &CPU::absolute,
              &CPU::absolute,     &CPU::absolute,     &CPU::illegal_mode,
              &CPU::relative,     &CPU::iny,          &CPU::illegal_mode,
              &CPU::illegal_mode, &CPU::illegal_mode, &CPU::zero_x,
              &CPU::zero_x,       &CPU::illegal_mode, &CPU::implied,
              &CPU::abs_y,        &CPU::illegal_mode, &CPU::illegal_mode,
              &CPU::illegal_mode, &CPU::abs_x,        &CPU::abs_x,
              &CPU::illegal_mode};
    int cycle_number[256] = {
        7, 6, 1, 1, 1, 3, 5, 1, 3, 2, 2, 1, 1, 4, 6, 1, 2, 5, 1, 1, 1, 4, 6, 1,
        2, 4, 1, 1, 1, 4, 7, 1, 6, 6, 1, 1, 3, 3, 5, 1, 4, 2, 2, 1, 4, 4, 6, 1,
        2, 5, 1, 1, 1, 4, 6, 1, 2, 4, 1, 1, 1, 4, 7, 1, 6, 6, 1, 1, 1, 3, 5, 1,
        3, 2, 2, 1, 3, 4, 6, 1, 2, 5, 1, 1, 1, 4, 6, 1, 2, 4, 1, 1, 1, 4, 7, 1,
        6, 6, 1, 1, 1, 3, 5, 1, 4, 2, 2, 1, 5, 4, 6, 1, 2, 5, 1, 1, 1, 4, 6, 1,
        2, 4, 1, 1, 1, 4, 7, 1, 1, 6, 1, 1, 3, 3, 3, 1, 2, 1, 2, 1, 4, 4, 4, 1,
        2, 6, 1, 1, 4, 4, 4, 1, 2, 5, 2, 1, 1, 5, 1, 1, 2, 6, 2, 1, 3, 3, 3, 1,
        2, 2, 2, 1, 4, 4, 4, 1, 2, 5, 1, 1, 4, 4, 4, 1, 2, 4, 2, 1, 4, 4, 4, 1,
        2, 6, 1, 1, 3, 3, 5, 1, 2, 2, 2, 1, 4, 4, 6, 1, 2, 5, 1, 1, 1, 4, 6, 1,
        2, 4, 1, 1, 1, 4, 7, 1, 2, 6, 1, 1, 3, 3, 5, 1, 2, 2, 2, 1, 4, 4, 6, 1,
        2, 5, 1, 1, 1, 4, 6, 1, 2, 4, 1, 1, 1, 4, 7, 1};
};
