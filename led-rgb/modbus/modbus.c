#include "modbus.h"

// ------------------------------------------------------------ //
uint16_t registerMap[MAX_REG + 1];

extern volatile uint8_t modbusAddress;

extern volatile uint8_t t0ovf;
extern volatile uint8_t newFrame;
extern volatile uint8_t endFrame;

extern volatile uint8_t frame[16];
extern volatile uint8_t frameIndex;

extern volatile uint8_t flag;
extern volatile uint8_t rsInFlag;

extern void bootload();

// ------------------------------------------------------------ //
void modbusReset() {
	t0ovf = 0;

	newFrame = 1;
	endFrame = 0;
	frameIndex = 0;

	flag = FLAG_OK;
}

void modbusTimerInit() {
	modbusReset();

	/* Overflow interrupt enable */
	TIMSK |= (1<<TOIE0);

	sei();
}

uint16_t make16Bit(uint8_t hi, uint8_t lo) {
	return (hi<<8) | lo;
}

// ------------------------------------------------------------ //
void modbusReply(uint8_t len, ...) {
	if (!frame[0] && !registerMap[LOCKDOWN_REGISTER])
		return;

	uint8_t reply[16];
	reply[0] = modbusAddress;

	va_list arg;
	va_start(arg, len);

	for (uint8_t i=0; i<len; i++) {
		reply[i+1] = (uint8_t)(va_arg(arg, int) & 0xFF);
	}

	va_end(arg);

	uint16_t crc = fastCRC(reply, 1+len);

	reply[1+len]   = (crc>>8) & 0xFF;
	reply[1+len+1] =  crc & 0xFF;

	// RS485_OUT();

	for (uint8_t i=0; i<len+3; i++)
		uartSend(reply[i]);

	rsInFlag = 1;

	// _delay_ms(1);
	// RS485_IN();
}

void modbusReplyError(uint8_t error) {
	switch (error) {
		case ILLEGAL_DATA_ADDRESS:
			/*
			 * 0x86 - Error code for function 0x06
			 * 0x02 - Illegal Data Address
			 *
			 */

			modbusReply(2, 0x80 + frame[0], 0x02);
			break;

		case ILLEGAL_DATA_VALUE:
			/*
			 * 0x86 - Error code for function 0x06
			 * 0x03 - Illegal Data Value
			 *
			 */

			modbusReply(2, 0x80 + frame[0], 0x03);
			break;
	}
}

void modbusEchoRequest(uint8_t upTo) {
	if (frame[0]) {
		for (uint8_t i=0; i<upTo; i++)
			uartSend(frame[i]);

		if (upTo != frameIndex) {
			uint16_t crc = fastCRC(frame, upTo);

			uartSend((crc>>8) & 0xFF);
			uartSend(crc & 0xFF);
		}

		rsInFlag = 1;
	}
}

// ------------------------------------------------------------ //
void handleFrame() {
	// uartSend(0xF0 + frame[1]);

	/* Function code */
	switch (frame[1]) {
		case 0x05:
			writeSingleCoil();
			break;

		case 0x06:
			writeSingleRegister();
			break;

		case 0x10:
			writeMultipleRegisters();
			break;

		default:
			// Illegal function (0x01)
			if (frame[0] && !registerMap[LOCKDOWN_REGISTER])
				modbusReply(2, frame[1]+0x80, 0x01);
	}
}

void registerMapUpdate(uint8_t registerAddress) {
	if (registerAddress <= 0x02)
		updatePWM(registerAddress);

	else if (registerAddress == DIM_REGISTER) {
		dim(registerMap[DIM_REGISTER]);
	}
}

void writeSingleCoil() {
	uint16_t registerAddress = make16Bit(frame[2], frame[3]);
	uint16_t registerValue = make16Bit(frame[4], frame[5]);


	if (registerAddress == BOOTLOAD_REGISTER) {
		if (registerValue == 0xFF00) {
			bootload();
		}

		else
			modbusReplyError(ILLEGAL_DATA_VALUE);
	}

	if (registerAddress != LOCKDOWN_REGISTER && registerMap[LOCKDOWN_REGISTER]) {
		RS485_IN();
		return;
	}


	if (registerAddress == LOCKDOWN_REGISTER) {

		switch (registerValue) {
			case 0xFF00:
				registerMap[registerAddress] = 1;
				break;

			case 0x0000:
				registerMap[registerAddress] = 0;
				break;

			default:
				modbusReplyError(ILLEGAL_DATA_VALUE);
				return;
		}

		// registerMapUpdate(registerAddress);

		modbusEchoRequest(frameIndex);
	}

	else
		modbusReplyError(ILLEGAL_DATA_ADDRESS);
}

void writeSingleRegister() {
	uint16_t registerAddress = make16Bit(frame[2], frame[3]);
	uint16_t registerValue = make16Bit(frame[4], frame[5]);

	if (registerMap[LOCKDOWN_REGISTER]) {
		RS485_IN();
		return;
	}

	if (registerAddress <= 0x02 || registerAddress == DIM_REGISTER) {
		registerMap[registerAddress] = registerValue;

		if (!(registerValue & 0xFF00) || registerAddress == DIM_REGISTER)
			registerMapUpdate(registerAddress);

		/*
		 * All good, echo the request
		 */
		modbusEchoRequest(frameIndex);
	}

	else
		modbusReplyError(ILLEGAL_DATA_ADDRESS);

	/*if (registerValue > 255) {
		modbusReplyError(ILLEGAL_DATA_VALUE);

		return;
	}*/
}

void writeMultipleRegisters() {
	uint16_t startingRegisterAddress = make16Bit(frame[2], frame[3]);
	uint16_t registerCount = make16Bit(frame[4], frame[5]);
	uint16_t byteCount = frame[6];

	if (registerMap[LOCKDOWN_REGISTER]) {
		RS485_IN();
		return;
	}

	// 9 = 7 (address + func code + starting x2 + quantity x2 + bytecount) + 2 CRC
	if (byteCount != registerCount*2 || frameIndex != (9 + byteCount)) {
		modbusReplyError(ILLEGAL_DATA_VALUE);

		return;
	}

	if (startingRegisterAddress+registerCount > 0x03) {
		modbusReplyError(ILLEGAL_DATA_ADDRESS);

		return;
	}

	uint16_t reg;

	for (int i=0; i<registerCount; i++) {
		reg = make16Bit(frame[7 + i*2], frame[8 + i*2]);

		/*if (reg > 255) {
			modbusReplyError(ILLEGAL_DATA_VALUE);

			return;
		}*/

		registerMap[startingRegisterAddress + i] = reg & 0xFF;

		if (!(reg & 0xFF00))
			registerMapUpdate(startingRegisterAddress + i);
	}

	/*
	 * All good, echo the first 6 bytes of the request
	 */

	modbusEchoRequest(6);
}

// ------------------------------------------------------------ //
ISR(TIMER0_OVF_vect) {
	if (t0ovf > OVF_T35 && endFrame == 1) {

		flag &= (frameIndex >= 4);//	if (flag != FLAG_OK) return;
		flag &= (fastCRC(frame, frameIndex) == 0);//			if (flag != FLAG_OK) return;
		flag &= (frame[0] == modbusAddress || frame[0] == 0);//	if (flag != FLAG_OK) return;

		// Process frame
		if (flag == FLAG_OK) {
			if (frame[0])
				RS485_OUT();

			handleFrame();
		}

		MODBUS_TIMER_STOP;
		modbusReset();
	}

	if (t0ovf > OVF_T15 && newFrame == 0) {
		// If character is received after setting this flag
		// The frame is FLAG_NOK
		endFrame = 1;

		// Waiting for t3,5 to expire
	}

	t0ovf++;

	TCNT0 += TIMER0_START;
}
