#include "modbus.h"

// ------------------------------------------------------------ //
// MODBUS address of this device
uint8_t EEMEM modbusAddress = 0x01;

// Register map
volatile uint16_t registerMap[MAX_REGISTERS + 1];

// Number of timer0 overflows
volatile uint8_t t0ovf;

// Indicates whether the received byte is part of new frame
volatile uint8_t newFrame;

// Set on OVF_T15 expire, maximal char space in MODBUS
volatile uint8_t t15overflowed;

// Frame buffer
volatile uint8_t frame[16];

// Current frame index
volatile uint8_t frameIndex;

// Frame OK flag
volatile uint8_t flag;

// Next finished transfer is going to disable drive
volatile uint8_t rsDriveDisableFlag = 0;

volatile uint8_t eventCounter = 0;

// ------------------------------------------------------------ //
/**
 * Resets MODBUS timer overflow counter, newFrame, endFrame, frameIndex flags
 * and clears all error.
 */
void modbusReset() {
	MODBUS_TIMER_STOP;

	t0ovf = 0;

	newFrame = 1;
	t15overflowed = 0;
	frameIndex = 0;

	flag = FLAG_OK;
}

// Called from main.c to start the timer for the first time.
void modbusInit() {
	modbusReset();

	/* Overflow interrupt enable */
	TIMSK |= (1<<TOIE0);

	sei();
}

// Makes 16-bit digit from 2 x 8bit ones
uint16_t make16Bit(uint8_t hi, uint8_t lo) {
	return (hi<<8) | lo;
}

// Goes to bootloader
void bootload() {
	cli();

	asm volatile(
		"clr r30 \n\t"
		"ldi r31, 0x1C \n\t"
		"ijmp"
	);
}

// ------------------------------------------------------------ //
void modbusReply(uint8_t len, ...) {
	uint8_t reply[16];
	reply[0] = eeprom_read_byte(&modbusAddress);

	va_list arg;
	va_start(arg, len);

	for (uint8_t i=0; i<len; i++) {
		reply[i+1] = (uint8_t)(va_arg(arg, int) & 0xFF);
	}

	va_end(arg);

	uint16_t crc = fastCRC(reply, 1+len);

	reply[1+len]   = (crc>>8) & 0xFF;
	reply[1+len+1] =  crc & 0xFF;

	for (uint8_t i=0; i<len+3; i++)
		uartSend(reply[i]);

	rsDriveDisableFlag = 1;
}

void modbusReplyError(uint8_t error) {
	modbusReply(2, 0x80 + frame[1], error);
}

void modbusEchoRequest(uint8_t replyByteCount) {
	for (uint8_t i=0; i<replyByteCount; i++)
		uartSend(frame[i]);

	// If not sending back the whole frame, recalculate the CRC
	if (replyByteCount != frameIndex) {
		uint16_t crc = fastCRC(frame, replyByteCount);

		uartSend((crc>>8) & 0xFF);
		uartSend(crc & 0xFF);
	}

	rsDriveDisableFlag = 1;
}

// ------------------------------------------------------------ //
/**
 * Handles timer overflows, OVF_T35(3,5 char) is a end-of-frame overflow count,
 * when exceeded, the frame is being processed. OVF_T15(1,5 char)
 * is a deadline for last valid character.
 */
ISR(TIMER0_OVF_vect) {
	TCNT0 += TIMER0_START;

	// When OVF_T15 occurred, and it's time for OVF_T35, then, execute.
	if (t0ovf > OVF_T35 && t15overflowed == 1) {

		// Flag is OK
		// Received at least 4 bytes (address, function code, 2x CRC)
		// CRC check turns out positive

		if (flag && frameIndex >= 4 && fastCRC(frame, frameIndex) == 0) {

			// Slave address matches
			if (frame[0] == eeprom_read_byte(&modbusAddress)) {
				RS485_OUT();
				handleFrame();
			}

			// it's a broadcast address
			else if (frame[0] == 0) {
				handleFrame();
			}
		}

		// Stop this timer
		modbusReset();
	}

	if (t0ovf > OVF_T15 && newFrame == 0) {
		// If character is received after setting this flag
		// The frame is FLAG_NOK
		t15overflowed = 1;

		// Waiting for t3,5 to expire
	}

	t0ovf++;
}

// Interrupt called when character is received.
ISR(USART_RXC_vect) {
	frame[frameIndex++] = UDR;

	t0ovf = 0;

	// If character is received after setting this flag
	// frame isn't valid.
	if (t15overflowed) {
		flag = FLAG_NOK;
	}

	// New frame is received, start the timers
	if (newFrame) {
		newFrame = 0;
		MODBUS_TIMER_START;
	}
}

// Disables RS485 transmitter when the flag is set
ISR(USART_TXC_vect) {
	if (rsDriveDisableFlag) {
		RS485_IN();

		rsDriveDisableFlag = 0;
	}
}

// ------------------------------------------------------------ //
// Handle different instructions
void handleFrame() {
	if (dimEnabled && frame[1] != GET_COMM_EVENT_COUNTER) {
		modbusReplyError(SERVER_DEVICE_BUSY);
	}

	else {
		switch (frame[1]) {
			case WRITE_SINGLE_COIL:
				writeSingleCoil();
				break;

			case WRITE_SINGLE_REGISTER:
				writeSingleRegister(make16Bit(frame[2], frame[3]), make16Bit(frame[4], frame[5]), 0);
				break;

			case GET_COMM_EVENT_COUNTER:
				getCommEventCounter();
				break;

			case WRITE_MULTIPLE_REGISTERS:
				writeMultipleRegisters();
				break;

			default:
				modbusReplyError(ILLEGAL_FUNCTION);
		}
	}
}

void writeSingleCoil() {
	uint16_t registerAddress = make16Bit(frame[2], frame[3]);
	uint16_t registerValue = make16Bit(frame[4], frame[5]);

	if (registerValue == 0xFF00 || registerValue == 0x0000) {
		// Error occurred
		uint8_t error = 0;

		switch (registerAddress) {
			case COIL_LATCH:
				updatePWM(registerMap[REGISTER_RED], registerMap[REGISTER_GREEN], registerMap[REGISTER_BLUE]);
				break;

			case COIL_LOCKDOWN:
				// lockdown todo
				break;

			case COIL_BOOTLOAD:
				if (registerValue == 0xFF00) bootload();
				else modbusReplyError(ILLEGAL_DATA_VALUE);
				error = 1;

				break;

			default:
				modbusReplyError(ILLEGAL_DATA_ADDRESS);
				error = 1;
		}

		if (!error) {
			modbusEchoRequest(frameIndex);
			eventCounter++;
		}
	}

	else {
		modbusReplyError(ILLEGAL_DATA_VALUE);
	}
}

void writeSingleRegister(uint16_t registerAddress, uint16_t registerValue, uint8_t multiple) {
	// Error occurred
	uint8_t error = 0;

	switch (registerAddress) {
		case REGISTER_RED:
		case REGISTER_GREEN:
		case REGISTER_BLUE:
			registerMap[registerAddress] = registerValue;
			break;

		case REGISTER_DIM:
			dim(registerMap[REGISTER_RED], registerMap[REGISTER_GREEN], registerMap[REGISTER_BLUE], registerValue);

			break;

		case REGISTER_ADDRESS:
			// Check the MODBUS address range
			if (registerValue < 1 || registerValue > 247) {
				modbusReplyError(ILLEGAL_DATA_VALUE);
				error = 1;
			}

			else
				eeprom_update_byte(&modbusAddress, registerValue);

			break;

		default:
			if (!multiple)
				modbusReplyError(ILLEGAL_DATA_ADDRESS);
			error = 1;
	}

	if (!error && !multiple) {

		// Echo the request
		modbusEchoRequest(frameIndex);
		eventCounter++;
	}
}

void getCommEventCounter() {
	uint8_t status = dimEnabled ? 0xFF : 0x00;

	uint8_t cntLo = eventCounter & 0xFF;
	uint8_t cntHi = (eventCounter >> 8) & 0xFF;

	modbusReply(5, GET_COMM_EVENT_COUNTER, status, status, cntHi, cntLo);
}

void writeMultipleRegisters() {
	uint16_t startingRegisterAddress = make16Bit(frame[2], frame[3]);
	uint16_t registerCount = make16Bit(frame[4], frame[5]);
	uint8_t byteCount = frame[6];

	// Check if frame length matches and byteCount == registerCount*2
	// 9 means 7 (address + func code + startingRegisterAddress x2 + registerCount x2 + byteCount) + 2 CRC
	if (byteCount != registerCount*2 || frameIndex != (9 + byteCount)) {
		modbusReplyError(ILLEGAL_DATA_VALUE);

		return;
	}

	if (startingRegisterAddress+registerCount-1 > MAX_MULTIPLE_REGISTERS) {
		modbusReplyError(ILLEGAL_DATA_ADDRESS);

		return;
	}

	for (int i=0; i<registerCount; i++) {
		uint16_t registerValue = make16Bit(frame[7 + i*2], frame[8 + i*2]);

		writeSingleRegister(startingRegisterAddress + i, registerValue, 1);
	}

	// All good, echo the first 6 bytes of the request
	modbusEchoRequest(6);
	eventCounter++;
}
