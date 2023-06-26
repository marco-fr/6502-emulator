#include "cpu.hpp"
#include <iostream>

int main()
{
    Memory mem(65535);
    CPU cpu(&mem);
    cpu.print_registers();
    cpu.execute(1000);
    cpu.print_stack();
    cpu.print_registers();
    cpu.print_flags();
    return 0;
}