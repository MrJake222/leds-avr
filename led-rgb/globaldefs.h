#define MODBUS

#define MODULE_TYPE 	0x0001

// ------------------------------------------------------------ //
#define FLAG_OK 1
#define FLAG_NOK 0

// ------------------------------------------------------------ //

// Max directly mapped registers
#define MAX_REGISTERS		0x0003

// Max register address to be written by WRITE_MULTIPLE_REGISTERS command
#define MAX_MULTIPLE_REGISTERS	0x0003

// Red color register
#define REGISTER_RED		0x0000

// Green color register
#define REGISTER_GREEN		0x0001

// Blue color register
#define REGISTER_BLUE		0x0002

// Dims on undims the leds
#define REGISTER_DIM		0x0003

// Latches (ie. updates the PWM values) the color registers
#define COIL_LATCH			0x1000

// Stores current MODBUS address saved in EEPROM
#define REGISTER_ADDRESS	0xFFD0

// Stores Module ID for auto ModType guess
#define REGISTER_ADDRESS_RESET 0xFFD1

// Stores Module ID for auto ModType guess
#define REGISTER_MODULE_TYPE 0xFFD2

// If set to on, the system doesn't respond to comms
#define COIL_LOCKDOWN		0xFFE0

// If written to 0xFF00, the device reboots to bootloader
#define COIL_BOOTLOAD		0xFFF0

// ------------------------------------------------------------ //
#define READ_HOLDING_REGISTERS		0x03
#define WRITE_SINGLE_COIL			0x05
#define WRITE_SINGLE_REGISTER		0x06
#define GET_COMM_EVENT_COUNTER		0x0B
#define WRITE_MULTIPLE_REGISTERS 	0x10

#define ILLEGAL_FUNCTION		0x01
#define ILLEGAL_DATA_ADDRESS	0x02
#define ILLEGAL_DATA_VALUE		0x03
#define SERVER_DEVICE_BUSY		0x06
