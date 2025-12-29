#include "../include/cpu.h"
#include "../include/memory.h"

#include <cstring>

CPU::CPU(Memory& mem) : memory(mem)
{
    memset(R, 0, sizeof(R));
    SREG = 0;
    PC = 0;
    SP = 0;
}

void CPU::step()		//pełne wykonanie jednej instrukcji
{

}

    //metody pomocnicze do step() - np. dekodowanie instrukcji, wykonanie instrukcji procesora

    //operacje na stosie - push i pop

    //getter i setter flag w SREG