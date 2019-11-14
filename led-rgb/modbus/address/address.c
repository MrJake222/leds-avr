#include "address.h"


uint8_t readModbusAddress() {
	/* Gets MODBUS address from DIP switches
	 *
	 * Sets direction, pull up and
	 */

	/*uint8_t address = 0;

	A1_DIR  &= ~(1<<A1_NUM);
	A1_PORT |=  (1<<A1_NUM);

	A2_DIR  &= ~(1<<A2_NUM);
	A2_PORT |=  (1<<A2_NUM);

	A3_DIR  &= ~(1<<A3_NUM);
	A3_PORT |=  (1<<A3_NUM);

	A4_DIR  &= ~(1<<A4_NUM);
	A4_PORT |=  (1<<A4_NUM);

	_delay_ms(10);

	address |= (A1_PIN >> A1_NUM) & 0x01;
	address |= (A2_PIN >> A2_NUM) & 0x01 << 1;
	address |= (A3_PIN >> A3_NUM) & 0x01 << 2;
	address |= (A4_PIN >> A4_NUM) & 0x01 << 3;*/

	A1_DIR  &= ~(1<<A1_NUM);
	A1_PORT |=  (1<<A1_NUM);

	A2_DIR  &= ~(1<<A2_NUM);
	A2_PORT |=  (1<<A2_NUM);

	A3_DIR  &= ~(1<<A3_NUM);
	A3_PORT |=  (1<<A3_NUM);

	A4_DIR  &= ~(1<<A4_NUM);
	A4_PORT |=  (1<<A4_NUM);

	uint8_t address = 0;
	address |=  (A1_PIN >> A1_NUM) & 0x01;
	address |= ((A2_PIN >> A2_NUM) & 0x01) << 1;
	address |= ((A3_PIN >> A3_NUM) & 0x01) << 2;
	address |= ((A4_PIN >> A4_NUM) & 0x01) << 3;

	return (~address & 0x0F) + 0x01;
}
