#include "memory.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>

void Memory::init_memory(U16 size)
{
    try
    {
        memory = new U8[size];
        mem_size = size;
        std::cout << size << " bytes allocated" << std::endl;
    }
    catch (std::bad_alloc&)
    {
        std::cout << "Failed to allocate " << size << " bytes of memory."
                  << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Memory::clear_memory()
{
    for (U16 i = 0; i < mem_size; i++)
    {
        memory[i] = 0;
    }
}

void Memory::load_bin_file()
{
    std::ifstream bin_file("data.bin", std::ios::in | std::ios::binary);
    if (!bin_file)
    {
        std::cout << "Cannot open binary file" << std::endl;
    }
    uint8_t byte;
    char tmp;
    uint16_t counter = 0x0600;
    while (bin_file)
    {
        bin_file.get(tmp);
        byte = tmp;
        memory[counter] = byte;
        counter++;
    }
    bin_file.close();
}

Memory::Memory(U16 size)
{
    init_memory(size);
    clear_memory();
}

Memory::~Memory() { delete memory; }