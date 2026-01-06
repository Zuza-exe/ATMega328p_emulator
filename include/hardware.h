#pragma once
#include <stdint.h>

constexpr uint8_t PINB  = 0x03;
constexpr uint8_t DDRB  = 0x04;
constexpr uint8_t PORTB = 0x05;

constexpr uint8_t PINC  = 0x06;
constexpr uint8_t DDRC  = 0x07;
constexpr uint8_t PORTC = 0x08;

constexpr uint8_t PIND  = 0x09;
constexpr uint8_t DDRD  = 0x0A;
constexpr uint8_t PORTD = 0x0B;

constexpr uint8_t SREG = 0x3F;
constexpr uint8_t SPH  = 0x3E;
constexpr uint8_t SPL  = 0x3D;


// Arduino UNO LED
constexpr uint8_t LED_PORT = PORTB;
constexpr uint8_t LED_BIT  = 5;
