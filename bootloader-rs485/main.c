#include <avr/io.h>
#include <avr/boot.h>
//#include <avr/wdt.h>
#include <util/delay.h>

#include "uart/uart.h"

#define APP_END		0x1C00
#define APP_PAGES	APP_END / SPM_PAGESIZE

uint8_t initSeq[2];

void app() {
	cli();

	asm volatile(
		"clr r30 \n\t"
		"clr r31 \n\t"
		"ijmp"
	);
}

void writePage(uint16_t page, uint8_t *buf) {
	cli();

	eeprom_busy_wait();

	boot_page_erase(page);
	boot_spm_busy_wait();

	// Fill the pagebuffer
	for (uint8_t i=0; i<SPM_PAGESIZE; i+=2) {
		uint16_t fill = *buf++;
		fill |= *buf++ << 8;

		boot_page_fill(i, fill);
	}

	boot_page_write(page);
	boot_spm_busy_wait();

	boot_rww_enable();
}

void sendReceive() {
	RS485_OUT();
	uartSend('+');

	_delay_us(200);
	RS485_IN();
}

int main(void) {
	uartInit();

	DDRB |= (1<<PB1);

	sendReceive();

	for (uint8_t i=0; i<2; i++)
		initSeq[i] = uartRecvTimeout(10);

	if (initSeq[0] != 0xAC || initSeq[1] != 0xDC)
		app();

	// -------------------------------------------------------------- //
	uint8_t pageCount = uartRecv();
	uint8_t pagebuf[SPM_PAGESIZE];

	sendReceive();

	// Read all pages
	for (uint8_t page=0; page<pageCount; page++) {
		uint16_t pageAddress = uartRecv() * SPM_PAGESIZE;

		// Read bytes in page
		for (uint8_t byte=0; byte<SPM_PAGESIZE; byte++) {
			pagebuf[byte] = uartRecv();
		}

		writePage(pageAddress, pagebuf);

		sendReceive();
	}

	sendReceive();

	app();
}

