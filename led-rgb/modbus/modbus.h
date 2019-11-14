#define MODBUS

#include <stdarg.h>
#include <stdint.h>

#include  "../crc/crc.h"
#include  "../pwm/pwm.h"
#include  "../led/led.h"

#ifndef UART
	#include  "../uart/uart.h"
#endif

// ------------------------------------------------------------ //
#define MAX_REG				0x0005
#define LOCKDOWN_REGISTER	0x0003
#define BOOTLOAD_REGISTER	0x0004
#define DIM_REGISTER		0x0005

// ------------------------------------------------------------ //

/* Offset of Timer0 TCNT0 */
#define TIMER0_START 0

/* Number of overflows for 1.5 and 3.5 characters
 * See attached timerFreq.ods for details */
#define OVF_T15 4
#define OVF_T35 9

// ------------------------------------------------------------ //
#define MODBUS_TIMER_START TCNT0 = TIMER0_START; TCCR0 = (1<<CS00)
#define MODBUS_TIMER_STOP  TCCR0 = 0

// ------------------------------------------------------------ //
#define ILLEGAL_DATA_ADDRESS	0x02
#define ILLEGAL_DATA_VALUE		0x03

// ------------------------------------------------------------ //
void modbusReset();
void modbusTimerInit();

// ------------------------------------------------------------ //
void modbusReply(uint8_t len, ...);
void modbusReplyError(uint8_t error);

void modbusEchoRequest(uint8_t upTo);

// ------------------------------------------------------------ //
void handleFrame();

void writeSingleCoil();			// 0x03
void writeSingleRegister();		// 0x06
void writeMultipleRegisters();	// 0x10

// ------------------------------------------------------------ //
ISR(TIMER0_OVF_vect);
