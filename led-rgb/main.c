#include <avr/io.h>
#include <util/delay.h>

#include "modbus/modbus.h"
#include "uart/uart.h"
#include "pwm/pwm.h"

volatile uint8_t dimEnabled = 0;
volatile uint8_t pwmMaxValue;
volatile uint8_t registerRed;
volatile uint8_t registerGreen;
volatile uint8_t registerBlue;
volatile uint8_t lightingUp;
volatile uint8_t stepTime;

// Starts dimming process in the main loop
void dimStart(uint8_t _pwmMaxValue, uint8_t _registerRed, uint8_t _registerGreen, uint8_t _registerBlue, uint8_t _stepTime, uint8_t _lightingUp) {
	pwmMaxValue = _pwmMaxValue;
	registerRed = _registerRed;
	registerGreen = _registerGreen;
	registerBlue = _registerBlue;
	stepTime = _stepTime;
	lightingUp = _lightingUp;

	dimEnabled = 1;
}

int main(void) {
	uartInit();
	pwmInit();
	modbusInit();

	while(1) {

		if (dimEnabled) {
			for (uint8_t value=0; value<=pwmMaxValue; value++) {
				uint8_t val;

				if (lightingUp)
					val = value;
				else
					val = pwmMaxValue - value;

				RED   = gammaPwm(registerRed * val / 100);
				GREEN = gammaPwm(registerGreen * val / 100);
				BLUE  = gammaPwm(registerBlue * val / 100);

				delayms(stepTime);
			}

			dimEnabled = 0;
		}
	}
}
