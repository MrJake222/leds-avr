#include "uart.h"


// ------------------------------------------------------------ //
extern volatile uint8_t t0ovf;
extern volatile uint8_t newFrame;
extern volatile uint8_t endFrame;

extern volatile uint8_t frame[16];
extern volatile uint8_t frameIndex;

extern volatile uint8_t flag;
extern volatile uint8_t rsInFlag;

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
	UCSRB = (1<<RXEN) | (1<<TXEN) | (1<<RXCIE) | (1<<TXCIE);

	// 8bit, 1stop, even parity
	UCSRC = (1<<URSEL) /*| (1<<UPM1)*/ | (1<<UCSZ0) | (1<<UCSZ1);

	sei();
}

void uartSend(uint8_t byte) {
	// Wait for buffer ready
	while ( !(UCSRA & (1<<UDRE)) );

	// Send byte
	UDR = byte;
}

// ------------------------------------------------------------ //
ISR(USART_TXC_vect) {
	if (rsInFlag) {
		RS485_IN();

		rsInFlag = 0;
	}
}


ISR(USART_RXC_vect) {
	uint8_t data = UDR;

	t0ovf = 0;

	if (endFrame) {
		flag = FLAG_NOK;
	}

	if (newFrame) {
		// Receiving first character

		newFrame = 0;
		MODBUS_TIMER_START;
	}

	frame[frameIndex++] = data;
}


/*
void uartSend16(uint16_t data) {
	uartSend((uint8_t)(data>>8 & 0xFF));
	uartSend((uint8_t)(data & 0xFF));
}
*/
