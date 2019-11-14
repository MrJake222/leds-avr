#include "led.h"

extern uint16_t registerMap[MAX_REG + 1];

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

void dim() {
	uint8_t maxVal = getValue(registerMap[0], registerMap[1], registerMap[2]);

	uint16_t duration = registerMap[DIM_REGISTER] & 0x0FFF;
	uint8_t stepTime = duration / maxVal;

//	DDRD |= (1<<PD3);

	for (uint8_t value=0; value<=maxVal; value++) {
		uint8_t val;

		if (registerMap[DIM_REGISTER] & 0xF000)
			val = value;
		else
			val = maxVal - value;

//		uartSend(val);

//		PORTD ^= (1<<PD3);
		RED   = gammaPwm(registerMap[0] * val / 100);
		GREEN = gammaPwm(registerMap[1] * val / 100);
		BLUE  = gammaPwm(registerMap[2] * val / 100);
//		PORTD ^= (1<<PD3);

		delayms(stepTime);
	}
}
