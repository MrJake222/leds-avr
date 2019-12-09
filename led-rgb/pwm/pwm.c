#include "pwm.h"

void pwmInit() {
	/* Pins */
	DDRB |= (1<<PB1) | (1<<PB2);

	/* Non-inverting mode, 8bit Phase Correct PWM */
	TCCR1A = (1<<COM1A1) | (1<<COM1B1) | (1>>WGM10);

	/* Prescaler 8 */
	TCCR1B = (1<<CS11);

	// ------------------------------------------------------------ //
	/* Pins */
	DDRB |= (1<<PB3);

	/* Non-inverting mode, 8bit Phase Correct PWM, Prescaler 8 */
	TCCR2 = (1<<COM21) | (1<<WGM20) | (1<<CS21);
}

// ------------------------------------------------------------ //

void updatePWM(uint8_t registerRed, uint8_t registerGreen, uint8_t registerBlue) {
//	switch (registerAddress) {
//		case REGISTER_RED:
//			RED = gammaPwm(registerValue);
//			break;
//
//		case REGISTER_GREEN:
//			GREEN = gammaPwm(registerValue);
//			break;
//
//		case REGISTER_BLUE:
//			BLUE = gammaPwm(registerValue);
//			break;
//	}

	RED = gammaPwm(registerRed);
	GREEN = gammaPwm(registerGreen);
	BLUE = gammaPwm(registerBlue);
}

void dim(uint8_t registerRed, uint8_t registerGreen, uint8_t registerBlue, uint16_t registerDim) {
	uint8_t maxVal = getValue(registerRed, registerGreen, registerBlue);

	uint16_t duration = registerDim & 0x0FFF;
	uint8_t stepTime = duration / maxVal;

	dimStart(maxVal, registerRed, registerGreen, registerBlue, stepTime, (registerDim & 0xF000) != 0);

//	for (uint8_t value=0; value<=maxVal; value++) {
//		uint8_t val;
//
//		if (registerDim & 0xF000)
//			val = value;
//		else
//			val = maxVal - value;
//
//		RED   = gammaPwm(registerRed * val / 100);
//		GREEN = gammaPwm(registerGreen * val / 100);
//		BLUE  = gammaPwm(registerBlue * val / 100);
//
//		delayms(stepTime);
//	}
}

uint8_t gammaPwm(uint8_t val) {
	return val*val / 255;
}

void delayms(uint8_t time) {
	while (time--) {
		_delay_ms(1);
	}
}

uint8_t getMax(uint8_t a, uint8_t b, uint8_t c) {
	if (a > b) {
		if (c > a)
			return c;

		else
			return a;
	}

	else {
		if (c > b)
			return c;

		else
			return b;
	}
}

uint8_t getValue(uint8_t r, uint8_t g, uint8_t b) {
	uint16_t max = getMax(r,g,b);

	max *= 100;
	max /= 255;

	return max;
}

