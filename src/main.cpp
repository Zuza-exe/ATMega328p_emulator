#include "../include/cpu.h"
#include "../include/io_regs.h"
#include "../include/memory.h"
#include <iostream>
#include <string>

using namespace std;

int main()
{
    string bin_filename = "../tests/test_ldi.bin";

    Memory mem;
    CPU cpu(mem);

    mem.load_bin_to_flash(bin_filename);

    mem.print_flash();

    cpu.print_status();
    cpu.step();
    cpu.print_status();

    return 0;
}