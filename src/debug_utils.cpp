#include "../include/debug_utils.h"
#include "../include/hardware.h"

std::string reg_name(int r)
{
    return "r" + std::to_string(r);
}

std::string io_name(uint8_t a)
{
    switch (a)
    {
        case PORTB: return "PORTB";
        case DDRB:  return "DDRB";
        case PINB:  return "PINB";
        case PORTC: return "PORTC";
        case DDRC:  return "DDRC";
        case PINC:  return "PINC";
        case PORTD: return "PORTD";
        case DDRD:  return "DDRD";
        case PIND:  return "PIND";
        default:    return "IO[" + std::to_string(a) + "]";
    }
}
