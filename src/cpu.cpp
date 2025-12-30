#include "../include/cpu.h"
#include "../include/memory.h"

#include <cstring>
#include<iostream>

CPU::CPU(Memory& mem) : memory(mem)
{
    memset(r, 0, sizeof(r));
    sreg = 0;
    pc = 0;
    sp = 0;
}

void CPU::step()
{
    uint16_t opcode = memory.fetch_instruction(pc);
    pc++;

    decode_and_execute(opcode);
}

void CPU::print_status()
{
    cout<<"======Printing CPU status======="<<endl;
    cout<<"Registers:"<<endl;
    for (int i = 0; i<32; i++)
    {
        cout << "r[" << i << "]: 0x"<< std::hex << +r[i]<< std::dec << endl; //+r[i] = static_cast<int>(r[i])
    }
    cout<<"SREG: 0x"<<std::hex<<+sreg<<endl;
    cout<<std::dec<<"PC: 0x"<<std::hex<<+pc<<endl;
    cout<<std::dec<<"SP: 0x"<<std::hex<<+sp<<std::dec<<endl;
}

    //metody pomocnicze do step() - np. dekodowanie instrukcji, wykonanie instrukcji procesora

void CPU::decode_and_execute(uint16_t opcode)
{
    //decoding instruction

    //nop - 0000 0000 0000 0000
    if((opcode&0xFFFF) == 0x0000)
    {
        return;
    }

    //ldi - 1110 kkkk(old bits) rrrr(registers 16-31) kkkk(young bits)
    if((opcode&0xF000) == 0xE000)
    {
        uint8_t id = 16 + ((opcode >> 4) & 0x0F);
        uint8_t k = ((opcode >> 4) & 0xF0) | (opcode & 0x0F);
        r[id] = k;  
    }

    //mov - 0010 11rd dddd rrrr - copy register r to d (both 0-31)
    if((opcode&0xFC00) == 0x2C00)
    {
        uint8_t r_id = ((opcode>>5)&0x0010) | (opcode&0x000F);
        uint8_t d_id = ((opcode>>4) & 0x001F);
        r[d_id] = r[r_id];
    }
}

    //operacje na stosie - push i pop

    //getter i setter flag w SREG