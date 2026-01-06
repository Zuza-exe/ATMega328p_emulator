#include <avr/io.h>

int main(void)
{
    // Ustaw PB5 jako wyjście
    DDRB |= (1 << PB5);

    // Pierwsze toggle → LED ON
    PINB |= (1 << PB5);  

    // Drugie toggle → LED OFF
    PINB |= (1 << PB5);  

    // Trzecie toggle → LED ON
    PINB |= (1 << PB5);  

    // Czwarte toggle → LED OFF
    PINB |= (1 << PB5);  
}