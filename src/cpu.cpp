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

    //ldi - 1110 kkkk(old bits) rrrr(registers 16-31) kkkk(young bits)
    if((opcode&0xF000) == 0xE000)
    {
        r[(opcode&0x00F0)>>4 | 16] = (opcode&0x0F00)>>4 | (opcode&0x000F);  
    }
}

    //operacje na stosie - push i pop

    //getter i setter flag w SREG