#include "../include/cpu.h"
#include "../include/memory.h"
#include "../include/io_regs.h"
#include "../include/debug.h"

#include <cstring>
#include<iostream>

CPU::CPU(Memory& mem) : memory(mem)
{
    memset(r, 0, sizeof(r));
    sreg = 0;
    pc = 0;
    sp = 2048;
}

void CPU::step()
{
    uint16_t opcode = memory.fetch_instruction(pc);
    pc++;

    decode_and_execute(opcode);

    DBG(cout<<"\n"<<hex<<opcode<<dec<<endl;)
    DBG(print_status();)
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
    cout<<std::dec<<"SP: 0x"<<std::hex<<+sp<<std::dec<<endl<<endl;;
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

    //============TODO:================
    //stos
    //logical operations
    //rol, ror...
    //out, toggle itd

    //adc - 0001 11rd dddd rrrr - Rd <- Rd + Rr + C
    if((opcode&0xFC00) == 0x1C00)
    {
        uint8_t r_id = ((opcode>>5)&0x0010) | (opcode&0x000F);
        uint8_t d_id = ((opcode>>4) & 0x001F);

        uint16_t sum = r[r_id] + r[d_id] + (uint16_t)get_flag(C);
        uint8_t result = sum & 0x00FF;

        //SETTING FLAGS

        //Carry
        set_flag(C, sum > 0x00FF);

        //Zero
        set_flag(Z, result == 0); //found information that it should be set_flag(Z, (result == 0) && get_flag(Z)); but I don't trust it

        //Negative
        set_flag(N, (result & 0x80) != 0);

        //Overflow
        set_flag(V, ((r[d_id] & 0x80) == (r[r_id] & 0x80)) && ((r[d_id] & 0x80) != (result & 0x80)));
        //set_flag(V, (~(r[d_id] ^ r[r_id]) & (r[d_id] ^ result) & 0x80));

        //Sign
        set_flag(S, get_flag(N) ^ get_flag(V));

        //Half-Carry
        set_flag(H,((r[d_id] & 0x0F) + (r[r_id] & 0x0F) + (uint8_t)get_flag(C)) > 0x0F);
        //set_flag(H,((r[d_id] ^ r[r_id] ^ result) & 0x10));

        r[d_id] = result;

        //print_status();

        return;
    }

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

        return;
    }
    
    // brxx instructions - 1111 0tkk kkkk ksss
    if ((opcode & 0xF000) == 0xF000)
    {
        uint8_t s = opcode & 0x7;          // which flag (sss)
        bool test_zero = opcode & 0x0400;  // t bit: 1 = branch if flag == 0

        int16_t offset = (int16_t)((opcode & 0x03F8) << 6) >> 6;

        bool flag = get_flag((Flag)s);

        if (test_zero ? !flag : flag)
            pc += offset;

        return;
    }

    //cp - 0001 01rd dddd rrrr - Rd - Rr
    if((opcode&0xFC00) == 0x1400)
    {
        uint8_t r_id = ((opcode>>5)&0x0010) | (opcode&0x000F);
        uint8_t d_id = ((opcode>>4) & 0x001F);

        uint8_t Rd = r[d_id];
        uint8_t Rr = r[r_id];
        uint8_t result = Rd - Rr;

        //SETTING FLAGS

        //Carry
        set_flag(C, Rd < Rr);

        //Zero
        set_flag(Z, result == 0);

        //Negative
        set_flag(N, result & 0x80);

        //Overflow
        set_flag(V, ((Rd & 0x80) != (Rr & 0x80)) && ((Rd & 0x80) != (result & 0x80)));   //Rr and Rd have different signs and result has different sign from Rd

        //Sign
        set_flag(S, get_flag(N) ^ get_flag(V));

        //Half-carry
        set_flag(H, (Rd&0x0F) < (Rr&0x0F));
        
        return;
    }

    //cpc - 0000 01rd dddd rrrr - Rd - Rr - C
    if((opcode&0xFC00) == 0x0400)
    {
        uint8_t r_id = ((opcode>>5)&0x0010) | (opcode&0x000F);
        uint8_t d_id = ((opcode>>4) & 0x001F);

        uint8_t carry_before = get_flag(C);
        uint8_t Rd = r[d_id];
        uint8_t Rr = r[r_id];
        uint8_t result = Rd - Rr - carry_before;

        //SETTING FLAGS

        //Carry
        set_flag(C, Rd < (Rr + carry_before));

        //Zero
        set_flag(Z, (result == 0) && get_flag(Z));

        //Negative
        set_flag(N, result & 0x80);

        //Overflow
        set_flag(V, ((Rd & 0x80) != (Rr & 0x80)) && ((Rd & 0x80) != (result & 0x80)));   //Rr and Rd have different signs and result has different sign from Rd

        //Sign
        set_flag(S, get_flag(N) ^ get_flag(V));

        //Half-carry
        set_flag(H, (Rd&0x0F) < ((Rr + carry_before)&0x0F));
        
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

    //rjmp - 1100 kkkk kkkk kkkk - PC <- PC + k + 1
    if((opcode&0xF000) == 0xC000)
    {
        uint16_t k = opcode & 0x0FFF;

        //filling with ones if k<0
        int16_t offset = (int16_t)(k << 4) >> 4;        //arithmetic shift

        pc += offset;    //not doing +1 because it's already done in step() method
        return;
    }

    //sbc - 0000 10rd dddd rrrr - Rd <- Rd - Rr - C
    if((opcode&0xFC00) == 0x0800)
    {
        uint8_t r_id = ((opcode>>5)&0x0010) | (opcode&0x000F);
        uint8_t d_id = ((opcode>>4) & 0x001F);

        uint8_t Rd = r[d_id];
        uint8_t Rr = r[r_id];

        uint8_t carry_before = get_flag(C);
        uint8_t result = Rd - Rr - carry_before;
        

        //SETTING FLAGS

        //Carry
        set_flag(C, Rd < (Rr + carry_before));

        //Zero
        set_flag(Z, (result == 0) && (get_flag(Z)));

        //Negative
        set_flag(N, result & 0x80);

        //Overflow
        set_flag(V, (((Rd & 0x80) != (Rr & 0x80)) && (Rd & 0x80) != (result & 0x80)));   //Rr and Rd have different signs and result has different sign from Rd

        //Sign
        set_flag(S, get_flag(N) ^ get_flag(V));

        //Half-carry
        set_flag(H, (Rd&0x0F) < ((Rr+carry_before)&0x0F));

        r[d_id] = result;

        return;
    }

    //sub - 0001 10rd dddd rrrr - Rd <- Rd - Rr
    if((opcode&0xFC00) == 0x1800)
    {
        uint8_t r_id = ((opcode>>5)&0x0010) | (opcode&0x000F);
        uint8_t d_id = ((opcode>>4) & 0x001F);

        uint8_t Rd = r[d_id];
        uint8_t Rr = r[r_id];
        uint8_t result = Rd - Rr;

        //SETTING FLAGS

        //Carry
        set_flag(C, Rd< Rr);

        //Zero
        set_flag(Z, result == 0);

        //Negative
        set_flag(N, result & 0x80);

        //Overflow
        set_flag(V, ((Rd & 0x80) != (Rr & 0x80)) && ((Rd & 0x80) != (result & 0x80)));   //Rr and Rd have different signs and result has different sign from Rd

        //Sign
        set_flag(S, get_flag(N) ^ get_flag(V));

        //Half-carry
        set_flag(H, (Rd&0x0F) < (Rr&0x0F));

        r[d_id] = result;

        return;
    }

    //unknown instruction
    std::cerr << "Unknown opcode: 0x"<< std::hex << opcode << std::dec << "\n";
}

    //operacje na stosie - push i pop

    //getter i setter flag w SREG