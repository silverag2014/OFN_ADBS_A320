
//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

// Device addresses (7 bits, lsb is a don't care)
#define I2C_ADDR    		(0x33 << 1)     // Device address for slave target
  
// Read/Write Command
#define I2C_WRITE          	0x00           	// SMBus WRITE command
#define I2C_READ           	0x01           	// SMBus READ command
                                     		// Note: This address is specified in the Microchip 24LC02B datasheet.
#define I2C_AUTO			0x80

// SMBus Buffer Size
#define I2C_BUFF_SIZE  		0x0A           	// Defines the maximum number of bytes that can be sent or received in a single transfer

// Status vector - top 4 bits only
#define I2C_MTSTA      		0xE0           	// (MT) start transmitted
#define I2C_MTDB       		0xC0           	// (MT) data byte transmitted
#define I2C_MRDB       		0x80           	// (MR) data byte received
// End status vector definition

//	i2c status
#define I2C_FREE			0x10
#define I2C_WRITE_EEPROM	0x04
#define I2C_READ_EEPROM		0x05

extern unsigned char i2c_status;
extern unsigned char i2c_buffer[I2C_BUFF_SIZE];

extern void i2c_Init(void);
extern void i2c_ByteWrite(unsigned char device_addr, unsigned char data_addr, unsigned char *dat, unsigned char num_byte);
extern void i2c_ByteRead(unsigned char device_addr, unsigned char data_addr, unsigned char *dat, unsigned char num_byte);
