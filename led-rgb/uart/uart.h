#define UART

#include <avr/io.h>
#include <avr/interrupt.h>

// ------------------------------------------------------------ //
#define BAUD 57600
#define BAUD_UBRR (F_CPU/16/BAUD - 1)

// ------------------------------------------------------------ //

void uartInit();
void uartSend(uint8_t byte);
