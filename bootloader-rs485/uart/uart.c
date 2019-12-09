#include "uart.h"


// ------------------------------------------------------------ //
volatile uint8_t rsInFlag;


// ------------------------------------------------------------ //
void uartInit() {
	// Set Tx pin as output
	DDRD |= (1<<PD1);

	// PD2 as output - direction control
	DDRD |= (1<<PD2);

	// Set speed for UART, see uart.h
	UBRRH = (uint8_t)(BAUD_UBRR>>8);
	UBRRL = (uint8_t)BAUD_UBRR;

	// Enable receiver and transmitter
	UCSRB = (1<<RXEN) | (1<<TXEN);

	// 8bit, 1stop, no parity
	UCSRC = (1<<URSEL) /*| (1<<UPM1)*/ | (1<<UCSZ0) | (1<<UCSZ1);

	RS485_IN();
}

void uartSend(uint8_t byte) {
	// Wait for buffer ready
	while ( !(UCSRA & (1<<UDRE)) );

	// Send byte
	UDR = byte;
}

uint8_t uartRecv() {
	while ( !(UCSRA & (1<<RXC)) );

	return UDR;
}

/* Timeout * 1 ms */
uint8_t uartRecvTimeout(int timeout) {
	uint8_t t = 0;

	while ( !(UCSRA & (1<<RXC)) ) {
		if (t == timeout)
			break;

		_delay_ms(100);
		PORTB ^= (1<<PB1);
		t++;
	}

	return UDR;
}
