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
    //jmp
    //rol, ror...
    //out, toggle, sbi itd

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
        set_flag(N, (result & 0x80));

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
        set_flag(N, (result & 0x80));

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
    
    //and - 0010 00rd dddd rrrr
    if((opcode&0xFC00) == 0x2000)
    {
        uint8_t r_id = ((opcode>>5)&0x0010) | (opcode&0x000F);
        uint8_t d_id = ((opcode>>4) & 0x001F);
        uint8_t result = r[r_id] & r[d_id];
        
        //SETTING FLAGS

        set_flag(Z, result == 0);
        set_flag(N, (result & 0x80));
        set_flag(V, false);
        set_flag(S, get_flag(N) ^ get_flag(V));

        r[d_id] = result;
        return;
    }

    // brxx instructions - 1111 0tkk kkkk ksss
    if((opcode&0xF000) == 0xF000)
    {
        uint8_t s = opcode & 0x7;          // which flag (sss)
        bool test_zero = opcode & 0x0400;  // t bit: 1 = branch if flag == 0

        int16_t offset = (int16_t)((opcode & 0x03F8) << 6) >> 6;

        bool flag = get_flag((Flag)s);

        if (test_zero ? !flag : flag)
            pc += offset;

        return;
    }

    //call - 1001 010k kkkk 111k | kkkk kkkk kkkk kkkk
    if((opcode&0xFE0E) == 0x940E)
    {
        uint16_t low = memory.fetch_instruction(pc);    //taking the argument
        pc++;

        uint32_t k = ((opcode&0x01F) << 13) | ((opcode&0x0001) << 16) | low;
        push(pc);
        pc = k;
    }

    //com - 1001 010d dddd 0000
    if((opcode&0xFC00) == 0x9400)
    {
        uint8_t d_id = ((opcode>>4) & 0x001F);
        uint8_t result = ~r[d_id];
        
        //SETTING FLAGS

        set_flag(C, true);
        set_flag(Z, result == 0);
        set_flag(N, (result & 0x80));
        set_flag(V, false);
        set_flag(S, get_flag(N) ^ get_flag(V));

        r[d_id] = result;
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

    //eor - 0010 01rd dddd rrrr
    if((opcode&0xFC00) == 0x2400)
    {
        uint8_t r_id = ((opcode>>5)&0x0010) | (opcode&0x000F);
        uint8_t d_id = ((opcode>>4) & 0x001F);
        uint8_t result = r[r_id] ^ r[d_id];
        
        //SETTING FLAGS

        set_flag(Z, result == 0);
        set_flag(N, (result & 0x80));
        set_flag(V, false);
        set_flag(S, get_flag(N) ^ get_flag(V));

        r[d_id] = result;
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

    //or - 0010 10rd dddd rrrr
    if((opcode&0xFC00) == 0x2800)
    {
        uint8_t r_id = ((opcode>>5)&0x0010) | (opcode&0x000F);
        uint8_t d_id = ((opcode>>4) & 0x001F);
        uint8_t result = r[r_id] | r[d_id];
        
        //SETTING FLAGS

        set_flag(Z, result == 0);
        set_flag(N, (result & 0x80));
        set_flag(V, false);
        set_flag(S, get_flag(N) ^ get_flag(V));

        r[d_id] = result;
        return;
    }

    //pop - 1001 000d dddd 1111
    if((opcode&0xFE0F) == 0x900F)
    {
        uint8_t reg = (opcode >> 4) & 0x1F;
        r[reg] = pop();
        return;
    }

    //push - 1001 001d dddd 1111
    if((opcode&0xFE0F) == 0x920F)
    {
        uint8_t reg = (opcode >> 4) & 0x1F;
        push (r[reg]);
        return;
    }

    //rcall - 1101 kkkk kkkk kkkk
    if((opcode&0xF000) == 0xD000)
    {
        uint16_t k = opcode & 0x0FFF; 
        int16_t offset = (int16_t)(k << 4) >> 4;

        push(pc);
        pc += offset;
    }

    //ret - 1001 0101 0000 1000
    if((opcode&0xFFFF) == 0x9508)
    {
        pc = pop16();
    }

    //reti - 1001 0101 0001 1000
    if((opcode&0xFFFF) == 0x9518)
    {
        pc = pop16();
        set_flag(I, true);

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

void CPU::push(uint8_t val)
{
    sp--;
    memory.set_sram(sp, val);
}

void CPU::push(uint16_t val)
{
    uint8_t h  = (val & 0xFF00) >> 8;
    uint8_t l = val & 0x00FF;

    push(l);
    push(h);
}

uint8_t CPU::pop()
{
    uint8_t val = memory.get_sram(sp);
    sp++;

    return val;
}

uint16_t CPU::pop16()
{
    uint8_t h = pop();
    uint8_t l = pop();

    return (uint16_t(h) << 8) | l;
}

/*

TODO KONIECZNE
blink.elf:     file format elf32-avr

00000068 <__ctors_end>:
  68:   11 24           eor     r1, r1
  6a:   1f be           out     0x3f, r1        ; 63
  6c:   cf ef           ldi     r28, 0xFF       ; 255
  6e:   d8 e0           ldi     r29, 0x08       ; 8
  70:   de bf           out     0x3e, r29       ; 62
  72:   cd bf           out     0x3d, r28       ; 61
  74:   0e 94 40 00     call    0x80    ; 0x80 <main>
  78:   0c 94 46 00     jmp     0x8c    ; 0x8c <_exit>

0000007c <__bad_interrupt>:
  7c:   0c 94 00 00     jmp     0       ; 0x0 <__vectors>

00000080 <main>:
  80:   20 9a           sbi     0x04, 0 ; 4
  82:   91 e0           ldi     r25, 0x01       ; 1
  84:   85 b1           in      r24, 0x05       ; 5
  86:   89 27           eor     r24, r25
  88:   85 b9           out     0x05, r24       ; 5
  8a:   fc cf           rjmp    .-8             ; 0x84 <main+0x4>

0000008c <_exit>:
  8c:   f8 94           cli

0000008e <__stop_program>:
  8e:   ff cf           rjmp    .-2             ; 0x8e <__stop_program>
*/