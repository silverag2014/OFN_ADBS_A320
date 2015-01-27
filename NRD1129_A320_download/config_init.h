
//;***************************************************************************
//;
//;        Copyright 2007 Avago Technologies Limited
//;    This code is provided by Avago Technologies as a reference.
//;     Avago Technologies makes no claims or warranties
//;    to this firmware's suitability for any application.
//;    All rights reserved. Patent Pending. 
//;
//;*************************************************************************** 

//#define FIRMWARE_VERSION	1.0

#define FIRMWARE_VERSION_MAJOR 2
#define FIRMWARE_VERSION_MINOR 84

#define CUSTOMER 		"A"
#define SAMPLE_TYPE 	"ES2"


#define RS232_CONSTANT
#define normal
#define SHUTTER_CALIBRATE
#define TAPPING
//#define MOTION_BURST



//------------------------------------------------------------------------------------
// Constant
//------------------------------------------------------------------------------------

#define LEFT_BUTTON		0x01
#define RIGHT_BUTTON	0x02
#define MIDDLE_BUTTON	0x04



//------------------------------------------------------------------------------------
// Pinout
//------------------------------------------------------------------------------------

#include <ADBM_A320.h>
#define PIXEL_NUMBER 		(19*19)
#define SPI_MODE			1
#define I2C_MODE			2

sbit SCLK		= P0^0;
sbit MISO		= P0^1;
sbit MOSI		= P0^2;
sbit NCS		= P0^3;
sbit DOME		= P0^5;

sbit MODE_SEL_SW = P1^0; 
sbit L1			= P1^1;	
sbit L2			= P1^2;
sbit L3 		= P1^3;
sbit L4			= P1^4;
sbit L5			= P1^5;	
sbit L6			= P1^6;
//sbit L7 		= P1^7; Modified by TLLAU
sbit LED7		= P1^7; // Modified by TLLAU

sbit NRESET		= P2^0;	// 0:reset, 1:- (active low)
sbit SHTDOWN 	= P2^1;	// 0:-, 1:shutdown (active high)
sbit MOTION 	= P2^2;
sbit GPIO 		= P2^3;
sbit ORIENT		= P2^4;	
sbit IO_SELECT 	= P2^5;	// 0:TWI; 1:SPI
sbit LEFT_SW 	= P2^6;
sbit RIGHT_SW	= P2^7;


extern bit L7; // Modified by TLLAU

void spi_enable(void);
void i2c_enable(void);
void Initiliaze_A320(void);
void Initiliaze_A320_I2C(void);


