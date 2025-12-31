#include "../include/cpu.h"
#include "../include/io_regs.h"
#include "../include/memory.h"
#include "../include/debug.h"
#include <iostream>
#include <string>

using namespace std;

int main()
{
    //string bin_filename = "../tests/test_ldi.bin";
    string bin_filename = "../tests/add_flag_tests.bin";

    Memory mem;
    CPU cpu(mem);

    DBG(cout<<"Debbuging mode on\n"<<endl;)

    mem.load_bin_to_flash(bin_filename);

    mem.print_flash();

    for(int i = 0; i < 10; i++)
    {
        cpu.step();
    }

    return 0;
}