#include "../include/cpu.h"
#include "../include/io_regs.h"
#include "../include/memory.h"
#include "../include/debug.h"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        cerr << "Usage: emulator <program.bin>\n";
        return 1;
    }

    string bin_filename = argv[1];

    Memory mem;
    CPU cpu(mem);

    DBG(cout<<"Debbuging mode on\n"<<endl;)

    if(mem.load_bin_to_flash(bin_filename))
    {
        DBG(mem.print_flash();)

        while(!cpu.get_halted())
        {
            cpu.step();
        }
    }
    return 0;
}