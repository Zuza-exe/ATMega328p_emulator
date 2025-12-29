#include <avr/io.h>

int main(void)
{
    DDRB |= (1 << PB0);   // ustaw PB0 jako wyjście

    while (1) {
        PORTB ^= (1 << PB0);  // zmień stan PB0 (toggle)
    }
}
