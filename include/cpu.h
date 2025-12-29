#pragma once

#include "memory.h"
#include<stdint.h>

//class Memory;

class CPU {
public:
    CPU(Memory& mem);

    void step();		//pełne wykonanie jednej instrukcji
private:
    Memory& memory;    

    uint8_t R[32];	//R0-R31 - rejestry ogólnego przeznaczenia
    uint8_t SREG;	//rejestr statusowy
    uint16_t PC;	//licznik programu
    uint16_t SP;	//wskaźnik stosu

    //metody pomocnicze do step() - np. dekodowanie instrukcji, wykonanie instrukcji procesora

    //operacje na stosie - push i pop

    //getter i setter flag w SREG

};