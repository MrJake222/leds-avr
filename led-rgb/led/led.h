#define LED

#include <util/delay.h>

#include "../pwm/pwm.h"
//#include "../uart/uart.h"

#ifndef MODBUS
#include "../modbus/modbus.h"
#endif

// ------------------------------------------------------------ //
#define STEP_TIME	10
//#define DURATION	1000
//
//#define STEPS		(DURATION / STEP_TIME)
//#define STEP		100 / STEPS

// ------------------------------------------------------------ //
uint8_t gammaPwm(uint8_t val);
void dim();
