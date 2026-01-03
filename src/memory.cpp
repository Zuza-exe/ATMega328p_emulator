#include "../include/memory.h"
#include "../include/io_regs.h"
#include <cstring>
#include <string>
#include<fstream>
#include<iostream>
#include<vector>

Memory::Memory()
{
    //setting memories to 0

    memset(flash, 0, sizeof(flash));
    memset(ioreg, 0, sizeof(ioreg));
    memset(ext_ioreg, 0, sizeof(ext_ioreg));
    memset(sram, 0, sizeof(sram));
}

void Memory::print_flash(size_t words)
{
    if(words>16*1024)
    {
        cerr<<"Error: Requested to print more words than there are in the flash memory"<<endl;
        return;
    }

    cout << "\nFlash memory (first " << words << " words):\n";
    cout << std::hex;

    for (size_t i = 0; i < words; i++)
    {
        cout << flash[i] << " ";
    }
    cout << std::dec << endl;
}


bool Memory::load_bin_to_flash(const std::string& path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file)
    {
        std::cerr << "Error: Couldn't open file "<<path<<endl;
        return false;
    }
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (size % 2 != 0)
    {
        std::cerr << "Invalid binary size (not word-aligned)\n";
        return false;
    }

    size_t wordCount = size / 2;
    size_t maxWords = sizeof(flash) / sizeof(flash[0]);

    if (wordCount > maxWords)
    {
        std::cerr << "Program too large for flash\n";
        return false;
    }

    std::vector<uint8_t> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
    {
        return false;
    }

    for (size_t i = 0; i < wordCount; ++i)
    {
        flash[i] = buffer[2*i] | (buffer[2*i + 1] << 8);
    }

    //print_flash();

    return true;
}




//zapis 1 do PINx → toggle odpowiedniego bitu w PORTx
void Memory::write_io(uint8_t addr, uint8_t value)
{
    switch (addr) {
    case PINB:
        ioreg[PORTB] ^= value;
        break;
    case PINC:
        ioreg[PORTC] ^= value;
        break;
    case PIND:
        ioreg[PORTD] ^= value;
        break;
    default:
        ioreg[addr] = value;
    }
}


//metody odczytu z pamięci

uint16_t Memory::fetch_instruction(uint16_t pc)
{
    return flash[pc];
}

uint8_t Memory::get_sram(uint16_t cell)
{
    return sram[cell];
}

void Memory::set_sram(uint16_t cell, uint8_t val)
{
    sram[cell] = val;
}