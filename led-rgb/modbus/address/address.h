#include <avr/io.h>
#include <util/delay.h>

#include <stdint.h>

// ------------------------------------------------------------ //

//#define PROTOTYPE

#ifndef PROTOTYPE
	#define A1_DIR		DDRC
	#define A1_PORT		PORTC
	#define A1_PIN		PINC
	#define A1_NUM		PC3

	#define A2_DIR		DDRC
	#define A2_PORT		PORTC
	#define A2_PIN		PINC
	#define A2_NUM		PC2

	#define A3_DIR		DDRC
	#define A3_PORT		PORTC
	#define A3_PIN		PINC
	#define A3_NUM		PC1

	#define A4_DIR		DDRC
	#define A4_PORT		PORTC
	#define A4_PIN		PINC
	#define A4_NUM		PC0

#else
	// Prototype 1

	#define A1_DIR		DDRC
	#define A1_PORT		PORTC
	#define A1_PIN		PINC
	#define A1_NUM		PC1

	#define A2_DIR		DDRC
	#define A2_PORT		PORTC
	#define A2_PIN		PINC
	#define A2_NUM		PC0

	#define A3_DIR		DDRB
	#define A3_PORT		PORTB
	#define A3_PIN		PINB
	#define A3_NUM		PB5

	#define A4_DIR		DDRB
	#define A4_PORT		PORTB
	#define A4_PIN		PINB
	#define A4_NUM		PB4
#endif

// ------------------------------------------------------------ //
uint8_t readModbusAddress();
