#include <avr/io.h>

int main(void)
{
    DDRB |= (1 << PB5);   // ustaw PB5 jako wyjście

    while (1) {
        PORTB ^= (1 << PB5);  // zmień stan PB5 (toggle)
    }
}
