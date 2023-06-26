#include <iostream>

#define U8 uint8_t
#define U16 uint16_t

class Memory
{
public:
    U16 mem_size;
    U8* memory;

    void init_memory(U16 size);
    void clear_memory();
    void load_bin_file();
    Memory(U16 size);
    ~Memory();
};