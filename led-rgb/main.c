#include <avr/io.h>
#include <util/delay.h>

#include "modbus/address/address.h"
#include "modbus/modbus.h"
#include "uart/uart.h"
#include "pwm/pwm.h"

// ------------------------------------------------------------ //

uint8_t modbusAddress = 0;

// Number of timer0 overflows
volatile uint8_t t0ovf;

// Indicates whether the received byte is part of new frame
// Then, start modbus timer
volatile uint8_t newFrame;

// Set on T15 expire, maximal char space in modbus
volatile uint8_t endFrame;

// frame buffer
volatile uint8_t frame[16];

// current frame index
volatile uint8_t frameIndex;

// flag indicating correctness of frame
volatile uint8_t flag;

// When set, next transmission complete interrupt will cause
// RS485 driver disable
volatile uint8_t rsInFlag = 0;

void bootload() {
	cli();

	asm volatile(
		"clr r30 \n\t"
		"ldi r31, 0x1C \n\t"
		"ijmp"
	);
}

// ------------------------------------------------------------ //
int main(void) {
	uartInit();
	pwmInit();
	modbusTimerInit();

	modbusAddress = readModbusAddress();

	/*RS485_OUT();
	uartSend(modbusAddress);*/

	/*PORTD |= (1<<PD1);
	_delay_ms(10);

	RS485_OUT();
	_delay_ms(10);

	modbusReply(5, 0x06, 0x00, 0x03, 0x00, modbusAddress);
*/
	// PORTB = 2;
	// DDRD |= (1<<PD2) | (1<<PD3);

	while(1) {
		/*uartSend(TCCR2);
		_delay_ms(500);*/
		/*PORTB <<= 1;

		if (PORTB == 16)
			PORTB = 2;*/
	}
}
