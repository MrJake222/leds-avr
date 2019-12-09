#define MODBUS

#include <stdarg.h>
#include <stdint.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>

#include "../globaldefs.h"

#include  "../crc/crc.h"
#include  "../pwm/pwm.h"
#include  "../uart/uart.h"

// ------------------------------------------------------------ //
extern volatile uint8_t dimEnabled;

// ------------------------------------------------------------ //
/* Offset of Timer0 TCNT0 */
#define TIMER0_START 96

/* Number of overflows for 1.5 and 3.5 characters
 * See attached timerFreq.ods for details */
#define OVF_T15 6
#define OVF_T35 14

// ------------------------------------------------------------ //
#define MODBUS_TIMER_START TCNT0 = TIMER0_START; TCCR0 = (1<<CS00)
#define MODBUS_TIMER_STOP  TCCR0 = 0

#define RS485_OUT() PORTD |=  (1<<PD2)
#define RS485_IN()  PORTD &= ~(1<<PD2)

// ------------------------------------------------------------ //
void modbusReset();
void modbusInit();

// ------------------------------------------------------------ //
void modbusReply(uint8_t len, ...);
void modbusReplyError(uint8_t error);

void modbusEchoRequest(uint8_t upTo);

// ------------------------------------------------------------ //
void handleFrame();

void writeSingleCoil();
void writeSingleRegister(uint16_t registerAddress, uint16_t registerValue, uint8_t multiple);
void getCommEventCounter();
void writeMultipleRegisters();

// ------------------------------------------------------------ //
ISR(TIMER0_OVF_vect);
ISR(USART_RXC_vect);
ISR(USART_TXC_vect);
