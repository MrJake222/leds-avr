#include "uart.h"

void uartInit() {
	// Set Tx pin as output
	DDRD |= (1<<PD1);

	// PD2 as output - direction control
	DDRD |= (1<<PD2);

	// Set speed for UART, see uart.h
	UBRRH = (uint8_t)(BAUD_UBRR>>8);
	UBRRL = (uint8_t)BAUD_UBRR;

	// Enable receiver and transmitter
	UCSRB = (1<<RXEN) | (1<<TXEN) | (1<<RXCIE) | (1<<TXCIE);

	// 8bit, 1stop, no parity
	UCSRC = (1<<URSEL) /*| (1<<UPM1)*/ | (1<<UCSZ0) | (1<<UCSZ1);

	sei();
}

void uartSend(uint8_t byte) {
	// Wait for buffer ready
	while ( !(UCSRA & (1<<UDRE)) );

	// Send byte
	UDR = byte;
}
