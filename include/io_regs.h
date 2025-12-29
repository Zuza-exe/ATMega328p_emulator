#pragma once

#include <stdint.h>

enum IORegister : uint8_t {
    PINB  = 0x03,
    DDRB  = 0x04,
    PORTB = 0x05,

    PINC  = 0x06,
    DDRC  = 0x07,
    PORTC = 0x08,

    PIND  = 0x09,
    DDRD  = 0x0A,
    PORTD = 0x0B
};