#include "../include/cpu.h"
#include "../include/memory.h"
#include "../include/io_regs.h"

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
    std::cout << "SREG: ["
              << "I=" << get_flag(I) << " "
              << "T=" << get_flag(T) << " "
              << "H=" << get_flag(H) << " "
              << "S=" << get_flag(S) << " "
              << "V=" << get_flag(V) << " "
              << "N=" << get_flag(N) << " "
              << "Z=" << get_flag(Z) << " "
              << "C=" << get_flag(C)
              << "]" << std::endl;
    cout<<std::dec<<"PC: 0x"<<std::hex<<+pc<<endl;
    cout<<std::dec<<"SP: 0x"<<std::hex<<+sp<<std::dec<<endl;
}

void CPU::set_flag(Flag f, bool v)
{
    if (v)
    {
        sreg |=  (1 << f);
    }
    else
    {
        sreg &= ~(1 << f);
    }
}

bool CPU::get_flag(Flag f) const
{
    return (sreg >> f) & 1;
}

    //metody pomocnicze do step() - np. dekodowanie instrukcji, wykonanie instrukcji procesora

void CPU::decode_and_execute(uint16_t opcode)
{
    //decoding instruction
    //they're sorted alphabetically

    //add - 0000 11rd dddd rrrr - Rd <- Rd + Rr
    if((opcode&0xFC00) == 0x0C00)
    {
        uint8_t r_id = ((opcode>>5)&0x0010) | (opcode&0x000F);
        uint8_t d_id = ((opcode>>4) & 0x001F);

        uint16_t sum = r[r_id] + r[d_id];
        uint8_t result = sum & 0x00FF;
        
        //SETTING FLAGS

        //Carry
        set_flag(C, sum > 0x00FF);

        //Zero
        set_flag(Z, result == 0);

        //Negative
        set_flag(N, (result & 0x80) != 0);

        //Overflow
        set_flag(V, ((r[d_id] & 0x80) == (r[r_id] & 0x80)) && ((r[d_id] & 0x80) != (result & 0x80)));
        //set_flag(V, (~(r[d_id] ^ r[r_id]) & (r[d_id] ^ result) & 0x80));

        //Sign
        set_flag(S, get_flag(N) ^ get_flag(V));

        //Half-Carry
        set_flag(H,((r[d_id] & 0x0F) + (r[r_id] & 0x0F)) > 0x0F);
        //set_flag(H,((r[d_id] ^ r[r_id] ^ result) & 0x10));

        r[d_id] = result;

        print_status();

        return;
    }
    

    //ldi - 1110 kkkk(old bits) rrrr(registers 16-31) kkkk(young bits)
    if((opcode&0xF000) == 0xE000)
    {
        uint8_t id = 16 + ((opcode >> 4) & 0x000F);
        uint8_t k = ((opcode >> 4) & 0x00F0) | (opcode & 0x000F);
        r[id] = k;  
        return;
    }

    //mov - 0010 11rd dddd rrrr - copy register r to d (both 0-31)
    if((opcode&0xFC00) == 0x2C00)
    {
        uint8_t r_id = ((opcode>>5)&0x0010) | (opcode&0x000F);
        uint8_t d_id = ((opcode>>4) & 0x001F);
        r[d_id] = r[r_id];
        return;
    }

    //nop - 0000 0000 0000 0000
    if((opcode&0xFFFF) == 0x0000)
    {
        return;
    }

    //unknown instruction
    std::cerr << "Unknown opcode: 0x"<< std::hex << opcode << std::dec << "\n";
}

    //operacje na stosie - push i pop

    //getter i setter flag w SREG