#pragma once

#include "memory.h"
#include<stdint.h>

//class Memory;

class CPU {
public:
    CPU(Memory& mem);

    void step();		//pełne wykonanie jednej instrukcji
    bool get_halted();
    void print_status();
    
private:
    Memory& memory;    

    uint8_t r[32];	//R0-R31 - rejestry ogólnego przeznaczenia
    uint8_t sreg;	//rejestr statusowy
    uint16_t pc;	//licznik programu
    uint16_t sp;	//shows TOP of the stack, not first free place

    bool cli_seen;   //to find sequence cli, rjmp
    bool halted;    //if finished working

    enum Flag {
        C = 0,
        Z = 1,
        N = 2,
        V = 3,
        S = 4,
        H = 5,
        T = 6,
        I = 7
    };

    void set_flag(Flag f, bool v);
    bool get_flag(Flag f) const;

    uint8_t get_data(uint16_t cell);
    void set_data(uint16_t cell, uint8_t val);

    bool is_two_word(uint16_t opcode);

    void decode_and_execute(uint16_t opcode);

    void push(uint8_t val);
    void push(uint16_t val);

    uint8_t pop();
    uint16_t pop16();
};