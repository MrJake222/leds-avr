/*
 * PWM library
 */


#include <avr/io.h>
#include <stdint.h>

#ifndef LED
#include "../led/led.h"
#endif

#ifndef MODBUS
#include "../modbus/modbus.h"
#endif


// ------------------------------------------------------------ //
#define RED OCR1B
#define GREEN OCR1A
#define BLUE OCR2

// ------------------------------------------------------------ //
void pwmInit();
void updatePWM(uint8_t registerAddress);
