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
extern uint16_t registerMap[MAX_REG + 1];

void updatePWM(uint8_t registerAddress) {
	switch (registerAddress) {
		case 0x00:
			RED   = gammaPwm(registerMap[0x00] & 0xFF);
			break;

		case 0x01:
			GREEN = gammaPwm(registerMap[0x01] & 0xFF);
			break;

		case 0x02:
			BLUE = gammaPwm(registerMap[0x02] & 0xFF);
			break;
	}
}
