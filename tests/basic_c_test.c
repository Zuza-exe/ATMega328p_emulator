#include <avr/io.h>
#include <stdint.h>

void toggle_led(void)
{
    // toggle PB5 przez zapis do PINB
    PINB = (1 << PB5);
}

int main(void)
{
    // PB5 jako wyjście
    DDRB |= (1 << PB5);
    PORTB &= ~(1 << PB5);   // LED OFF

    volatile uint8_t i = 3;

    while (i) {
        toggle_led();   // CALL / RET (32-bit CALL)
        i--;            // DEC
    }

    // LED powinien być w stanie ON (3 toggle)
    // koniec programu -> _exit -> cli; rjmp -1
}
