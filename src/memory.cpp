#include "../include/memory.h"
#include "../include/hardware.h"
#include "../include/debug_utils.h"
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

void Memory::print_io()
{
    cout<<"\nI/O regs:"<<endl;
    for(int i = 3; i<12; i++)
    {
        cout<<io_name(i)<<": "<<ioreg[i]<<endl;
    }
    cout<<endl;
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

uint16_t Memory::fetch_instruction(uint16_t pc)
{
    return flash[pc];
}

uint8_t Memory::get_sram(uint16_t cell) const
{
    return sram[cell];
}

void Memory::set_sram(uint16_t cell, uint8_t val)
{
    sram[cell] = val;
}

uint8_t Memory::get_io(uint8_t cell) const
{
    return ioreg[cell];
}

void Memory::set_io(uint8_t cell, uint8_t val)
{
    //PINx <- 1 - PORTx toggle
    switch (cell)
    {
    case PINB:
    {
        ioreg[PORTB] ^= val;
        update_led();
        break;
    }
    case PORTB:
    {
        ioreg[PORTB] = val;
        update_led();
        break;
    }
    case DDRB:
    {
        ioreg[DDRB] = val;
        update_led();
        break;
    }
    case PINC:
        ioreg[PORTC] ^= val;
        break;
    case PIND:
        ioreg[PORTD] ^= val;
        break;
    default:
        ioreg[cell] = val;  //no toggle
    }
}

bool Memory::get_io_bit(uint8_t cell, uint8_t bit) const
{
    return (ioreg[cell] >> bit) & 1;
}

void Memory::set_io_bit(uint8_t cell, uint8_t bit, bool val)
{
    uint8_t mask = 1 << bit;

    uint8_t new_val = ioreg[cell];
    if (val)
    {
        new_val |= mask;
    }
    else
    {
        new_val &= ~mask;
    }

    set_io(cell, new_val);
}

uint8_t Memory::get_data(uint16_t cell)
{
    if(cell < 32)
    {
        return 0;   //shouldn't be done by memory
    }
    if (cell < 96)
    {
        return ioreg[cell-32];
    }
    else if (cell < 256)
    {
        return ext_ioreg[cell-96];
    }
    else
    {
        return sram[cell-256];
    }
}

void Memory::set_data(uint16_t cell, uint8_t val)
{
    if(cell < 32)
    {
        return;
    }
    else if (cell < 96)
    {
        ioreg[cell-32] = val;
    }
    else if (cell < 256)
    {
        ext_ioreg[cell-96] = val;
    }
    else
    {
        sram[cell-256] = val;
    }
}

void Memory::update_led()
{
    bool led = (ioreg[PORTB] & (1 << LED_BIT)) && (ioreg[DDRB]  & (1 << LED_BIT));
    cout << "LED: " << (led ? "ON" : "OFF") << "\n";
}