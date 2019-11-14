#define UART

#include <avr/io.h>
#include <avr/interrupt.h>

#ifndef MODBUS
	#include "../modbus/modbus.h"
#endif

// ------------------------------------------------------------ //
#define FLAG_OK 1
#define FLAG_NOK 0

#define BAUD 57600
#define BAUD_UBRR (F_CPU/16/BAUD - 1)

#define RS485_OUT() PORTD |=  (1<<PD2)
#define RS485_IN()  PORTD &= ~(1<<PD2)

// ------------------------------------------------------------ //
void uartInit();
void uartSend(uint8_t byte);
//void uartSend16(uint16_t data);

ISR(USART_TXC_vect);
ISR(USART_RXC_vect);
