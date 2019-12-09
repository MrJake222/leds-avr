/*
 * PWM library
 */

#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

#include "../globaldefs.h"

// ------------------------------------------------------------ //
#define RED OCR1B
#define GREEN OCR1A
#define BLUE OCR2

#define STEP_TIME	10
// ------------------------------------------------------------ //

// Initialize the PWM
void pwmInit();

// Updates the Avr's PWMs register
void updatePWM(uint8_t registerRed, uint8_t registerGreen, uint8_t registerBlue);

// Dim the leds
void dim(uint8_t registerRed, uint8_t registerGreen, uint8_t registerBlue, uint16_t registerDim);
extern void dimStart(uint8_t _pwmMaxValue, uint8_t _registerRed, uint8_t _registerGreen, uint8_t _registerBlue, uint8_t _stepTime, uint8_t lightingUp);

// Calculate the gamma
uint8_t gammaPwm(uint8_t val);

// Wait given amount of time
void delayms(uint8_t time);

// Get the greatest value of 3 given
uint8_t getMax(uint8_t a, uint8_t b, uint8_t c);

// Get color value from RGB
uint8_t getValue(uint8_t r, uint8_t g, uint8_t b);
