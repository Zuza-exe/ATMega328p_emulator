#pragma once

#include "memory.h"
#include<stdint.h>

//class Memory;

class CPU {
public:
    CPU(Memory& mem);

    void step();		//pełne wykonanie jednej instrukcji
    void print_status();
    
private:
    Memory& memory;    

    uint8_t r[32];	//R0-R31 - rejestry ogólnego przeznaczenia
    uint8_t sreg;	//rejestr statusowy
    uint16_t pc;	//licznik programu
    uint16_t sp;	//wskaźnik stosu

    //metody pomocnicze do step() - np. dekodowanie instrukcji, wykonanie instrukcji procesora
    void decode_and_execute(uint16_t opcode);

    //operacje na stosie - push i pop

    //getter i setter flag w SREG

};