
//;***************************************************************************
//;
//;        Copyright 2007 Avago Technologies Limited
//;    This code is provided by Avago Technologies as a reference.
//;     Avago Technologies makes no claims or warranties
//;    to this firmware's suitability for any application.
//;    All rights reserved. Patent Pending. 
//;
//;*************************************************************************** 

#include "c8051f3xx.h"
#include <intrins.h>
#include <SPI_TRANSFER.h>
#include <i2c.h>
#include "timer.h"


unsigned char spi_temp;

extern unsigned char i2c_buffer[10];


void Initiliaze_A520(void);
void spi_enable(void);
void i2c_enable(void);
void Power_Up_Sequence(void);
void Initiliaze_A520_I2C(void);
void Power_Up_Sequence_I2C(void);
void i2c_delay(void);


void spi_enable(void)
{
	SMB0CF &= ~0x80;	// disable i2c
	EIP1   &= ~0x01;
	IO_SELECT = 1;		// enable spi communication

	SHTDOWN = 0;//-------------
	NRESET = 1;//-------------

	spi_init();
}


void i2c_enable(void)
{
	IO_SELECT = 0;		// enable i2c communication
	EIP1      |= 0x01;

	SHTDOWN = 0;
	NRESET = 1;

	MOSI = 0;			// device address A0
	NCS = 0;			// device address A1
	i2c_Init();			// enable i2c
}

void Initiliaze_A520(void)
{
	spi_enable();
	EA = 0;
	spi_temp = spi_read_sensor(ADBM_A520_PRODUCTID_ADDR);	
	Power_Up_Sequence();
	EA = 1;

	t_temp_data = 50;
	time_temp = 1;
	while(time_temp == 1);

	EA = 0;
	spi_write_sensor(ADBM_A520_CONFIGURATIONBITS_ADDR, 0x80);  // Configuration register data (Normal 800CPI)
	EA = 1;

}

void Power_Up_Sequence(void)
{
	NCS = 0;		// NCS Low
	SHTDOWN = 0;	// Shutdown Low
	ORIENT = 0;		// Orient Low
	IO_SELECT = 1;	// select SPI Mode
	NRESET = 0;		// NRESET Low
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	NRESET = 1;		// NRESET High


	ORIENT = 1;		// For OFN orientation

	// Perform soft reset by writing 0x5A to address 0x3A
	spi_write_sensor(ADBM_A520_SOFTRESET_ADDR, 0x5A);
	_nop_();
	_nop_();

	#ifdef MOTION_BURST
	spi_write_sensor(ADBM_A520_IO_MODE_ADDR, ADBM_A520_BURST);
	#endif

		// Enable OFN functions
	spi_write_sensor(0x60, 0xE4); 
	_nop_();
	_nop_();

	// speed switching thresholds
	spi_write_sensor(0x62, 0x1A); // Wakeup 500DPI
	_nop_();
	_nop_();

	spi_write_sensor(0x63, 0x0E); // 16ms, low DPI
	_nop_();
	_nop_();

	spi_write_sensor(0x64, 0x06); 
	_nop_();
	_nop_();

	spi_write_sensor(0x65, 0x03); 
	_nop_();
	_nop_();           

	spi_write_sensor(0x66, 0x08); 
	_nop_();
	_nop_(); 

	spi_write_sensor(0x67, 0x04); 
	_nop_();
	_nop_(); 

	spi_write_sensor(0x68, 0x10); 
	_nop_();
	_nop_(); 

	spi_write_sensor(0x69, 0x08); 
	_nop_();
	_nop_(); 

	// assert/ deassert thresholds
	spi_write_sensor(0x6D, 0xC4); 
	_nop_();
	_nop_();

	spi_write_sensor(0x6E, 0x44); 
	_nop_();
	_nop_(); 

	spi_write_sensor(0x6F, 0x51); 
	_nop_();
	_nop_(); 

	spi_write_sensor(0x70, 0x38); 
	_nop_();
	_nop_(); 

	spi_write_sensor(0x71, 0x44); 
	_nop_();
	_nop_(); 


	// FPD
	spi_write_sensor(0x75, 0x50); 
	_nop_();
	_nop_(); 

//	spi_write_sensor(0x76, 0x0F); 
//	_nop_();
//	_nop_(); 


	// XYQ
	spi_write_sensor(0x73, 0x99); 
	_nop_();
	_nop_(); 

	spi_write_sensor(0x74, 0x02); 
	_nop_();
	_nop_();

	// ##### LED Current #####
	spi_write_sensor(0x1A, 0x05); 
	_nop_();
	_nop_();

	// Read from registers 0x02, 0x03 and 0x04
	spi_temp = spi_read_sensor(0x02);
	_nop_();
	_nop_();
	spi_temp = spi_read_sensor(0x03);
	_nop_();
	_nop_();
	spi_temp = spi_read_sensor(0x04);
	_nop_();
	_nop_();

}


void Initiliaze_A520_I2C(void)
{
	EIE1 &= ~0x02;
	Power_Up_Sequence_I2C();
	EIE1 |= 0x02;

	t_temp_data = 50;
	time_temp = 1;
	while(time_temp == 1);

	EIE1 &= ~0x02;
	while(i2c_status != I2C_FREE);
	i2c_buffer[0] = 0x80;
	i2c_ByteWrite(I2C_ADDR, ADBM_A520_CONFIGURATIONBITS_ADDR, i2c_buffer, 1); // Configuration register data (Normal 800CPI)  
	while(i2c_status != I2C_FREE);
	EIE1 |= 0x02;

}

void Power_Up_Sequence_I2C(void)
{
	NCS = 0;		// NCS Low
	SHTDOWN = 0;	// Shutdown Low
	ORIENT = 0;		// Orient Low
	IO_SELECT = 0;	// select I2C Mode

	MOSI = 0;			// device address A0
	NCS = 0;			// device address A1

	NRESET = 0;		// NRESET Low
	i2c_delay();
	NRESET = 1;		// NRESET High

	ORIENT = 1;		// For OFN orientation

	EIP1      |= 0x01;
	i2c_Init();			// enable i2c

	i2c_delay();

	while(i2c_status != I2C_FREE);
	i2c_ByteRead(I2C_ADDR, ADBM_A520_PRODUCTID_ADDR, i2c_buffer, 1);	
	while(i2c_status != I2C_FREE);
	i2c_delay();


	// Perform soft reset by writing 0x5A to address 0x3A
	while(i2c_status != I2C_FREE); 
	i2c_buffer[0] = 0x5A;
	i2c_ByteWrite(I2C_ADDR, ADBM_A520_SOFTRESET_ADDR, i2c_buffer, 1); // Configuration register data (Normal 800CPI)  
	while(i2c_status != I2C_FREE);
	i2c_delay();


	while(i2c_status != I2C_FREE); 
	i2c_buffer[0] = 0xE4;
	i2c_ByteWrite(I2C_ADDR, 0x60, i2c_buffer, 1); // Write 0xA0 to address 0x60 
	while(i2c_status != I2C_FREE);
	i2c_delay();

	// speed switching thresholds
	while(i2c_status != I2C_FREE); 
	i2c_buffer[0] = 0x1A;
	i2c_ByteWrite(I2C_ADDR, 0x62, i2c_buffer, 1); // Wakeup 500DPI
	while(i2c_status != I2C_FREE);
	i2c_delay();

	while(i2c_status != I2C_FREE); 
	i2c_buffer[0] = 0x0E;
	i2c_ByteWrite(I2C_ADDR, 0x63, i2c_buffer, 1); // 16ms, low DPI
	while(i2c_status != I2C_FREE);
	i2c_delay();

	while(i2c_status != I2C_FREE); 
	i2c_buffer[0] = 0x06;
	i2c_ByteWrite(I2C_ADDR, 0x64, i2c_buffer, 1); 
	while(i2c_status != I2C_FREE);
	i2c_delay();
 
	while(i2c_status != I2C_FREE); 
	i2c_buffer[0] = 0x03;
	i2c_ByteWrite(I2C_ADDR, 0x65, i2c_buffer, 1); 
	while(i2c_status != I2C_FREE);
	i2c_delay();           

	while(i2c_status != I2C_FREE); 
	i2c_buffer[0] = 0x08;
	i2c_ByteWrite(I2C_ADDR, 0x66, i2c_buffer, 1); 
	while(i2c_status != I2C_FREE);
	i2c_delay(); 

	while(i2c_status != I2C_FREE); 
	i2c_buffer[0] = 0x04;
	i2c_ByteWrite(I2C_ADDR, 0x67, i2c_buffer, 1); 
	while(i2c_status != I2C_FREE);
	i2c_delay(); 

	while(i2c_status != I2C_FREE); 
	i2c_buffer[0] = 0x10;
	i2c_ByteWrite(I2C_ADDR, 0x68, i2c_buffer, 1); 
	while(i2c_status != I2C_FREE);
	i2c_delay(); 

	while(i2c_status != I2C_FREE); 
	i2c_buffer[0] = 0x08;
	i2c_ByteWrite(I2C_ADDR, 0x69, i2c_buffer, 1);
	while(i2c_status != I2C_FREE);
	i2c_delay();

	// assert/ deassert thresholds
	while(i2c_status != I2C_FREE); 
	i2c_buffer[0] = 0xC4;
	i2c_ByteWrite(I2C_ADDR, 0x6D, i2c_buffer, 1); 
	while(i2c_status != I2C_FREE);
	i2c_delay();

	while(i2c_status != I2C_FREE); 
	i2c_buffer[0] = 0x44;
	i2c_ByteWrite(I2C_ADDR, 0x6E, i2c_buffer, 1); 
	while(i2c_status != I2C_FREE);
	i2c_delay(); 

	while(i2c_status != I2C_FREE); 
	i2c_buffer[0] = 0x51;
	i2c_ByteWrite(I2C_ADDR, 0x6F, i2c_buffer, 1); 
	while(i2c_status != I2C_FREE);
	i2c_delay();
 
	while(i2c_status != I2C_FREE); 
	i2c_buffer[0] = 0x38;
	i2c_ByteWrite(I2C_ADDR, 0x70, i2c_buffer, 1);   
	while(i2c_status != I2C_FREE);
	i2c_delay(); 

	while(i2c_status != I2C_FREE); 
	i2c_buffer[0] = 0x44;
	i2c_ByteWrite(I2C_ADDR, 0x71, i2c_buffer, 1);   
	while(i2c_status != I2C_FREE);
	i2c_delay();

	// FPD
	while(i2c_status != I2C_FREE); 
	i2c_buffer[0] = 0x50;
	i2c_ByteWrite(I2C_ADDR, 0x75, i2c_buffer, 1); 
	while(i2c_status != I2C_FREE);
	i2c_delay(); 

//	while(i2c_status != I2C_FREE); 
//	i2c_buffer[0] = 0x0F;
//	i2c_ByteWrite(I2C_ADDR, 0x76, i2c_buffer, 1); 
//	while(i2c_status != I2C_FREE);
//	i2c_delay();


	// XYQ
	while(i2c_status != I2C_FREE); 
	i2c_buffer[0] = 0x99;
	i2c_ByteWrite(I2C_ADDR, 0x73, i2c_buffer, 1);  
	while(i2c_status != I2C_FREE);
	i2c_delay(); 

	while(i2c_status != I2C_FREE); 
	i2c_buffer[0] = 0x02;
	i2c_ByteWrite(I2C_ADDR, 0x74, i2c_buffer, 1); 
	while(i2c_status != I2C_FREE);
	i2c_delay();

	while(i2c_status != I2C_FREE); 
	i2c_buffer[0] = 0x05;
	i2c_ByteWrite(I2C_ADDR, 0x1A, i2c_buffer, 1); 
	while(i2c_status != I2C_FREE);
	i2c_delay();


	// Read from registers 0x02, 0x03 and 0x04
	while(i2c_status != I2C_FREE); 
	i2c_ByteRead(I2C_ADDR, ADBM_A520_MOTION_ADDR, i2c_buffer, 1);
	while(i2c_status != I2C_FREE);
	i2c_delay();

	while(i2c_status != I2C_FREE); 
	i2c_ByteRead(I2C_ADDR, ADBM_A520_DELTAX_ADDR, i2c_buffer, 1);
	while(i2c_status != I2C_FREE);
	i2c_delay();

	while(i2c_status != I2C_FREE); 
	i2c_ByteRead(I2C_ADDR, ADBM_A520_DELTAY_ADDR, i2c_buffer, 1);
	while(i2c_status != I2C_FREE);
	i2c_delay();

}

void i2c_delay(void)
{
	_nop_();_nop_();
}

