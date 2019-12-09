#define UART

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//#ifndef MODBUS
//	#include "../modbus/modbus.h"
//#endif

// ------------------------------------------------------------ //
#define BAUD 57600
#define BAUD_UBRR (F_CPU/16/BAUD - 1)

#define RS485_OUT() PORTD |=  (1<<PD2)
#define RS485_IN()  PORTD &= ~(1<<PD2)

// ------------------------------------------------------------ //
void uartInit();

void uartSend(uint8_t byte);
uint8_t uartRecv();
uint8_t uartRecvTimeout(int timeout);
//void uartSend16(uint16_t data);

ISR(USART_TXC_vect);
ISR(USART_RXC_vect);
