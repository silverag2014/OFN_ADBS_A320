//-----------------------------------------------------------------------------
// F34x_SMBus_EEPROM.c
//-----------------------------------------------------------------------------
// Copyright 2006 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This example demonstrates how the C8051F34x SMBus interface can communicate
// with a 256 byte I2C Serial EEPROM (Microchip 24LC02B).
// - Interrupt-driven SMBus implementation
// - Only master states defined (no slave or arbitration)
// - Timer1 used as SMBus clock source
// - Timer2 used by SMBus for SCL low timeout detection
// - SCL frequency defined by <SMB_FREQUENCY> constant
// - Pinout:
//    P0.0 -> SDA (SMBus)
//    P0.1 -> SCL (SMBus)
//
//    P2.2 -> LED
//
//    all other port pins unused
//
// How To Test:
//
// 1) Download code to a 'F34x device that is connected to a 24LC02B serial
//    EEPROM (see the EEPROM datasheet for the pinout information).
// 2) Run the code:
//         a) the test will indicate proper communication with the EEPROM by
//            turning on the LED at the end the end of the test
//         b) the test can also be verified by running to the if statements
//            in main and checking the sent and received values by adding
//            the variables to the Watch Window
//
// FID:            34X000078
// Target:         C8051F34x
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (TP)
//    -30 MAR 2006
//

//-----------------------------------------------------------------------------
// Includes and Device-Specific Parameters
//-----------------------------------------------------------------------------

#include <C8051F3xx.h>
#include <i2c.h>
#include <intrins.h>


//-----------------------------------------------------------------------------
// Global VARIABLES
//-----------------------------------------------------------------------------
unsigned char i2c_status;
unsigned char i2c_buffer[I2C_BUFF_SIZE]; 			// Holds the return value

unsigned char* p_i2c_data_in; 	// Global pointer for SMBus data. All receive data is written here

unsigned char i2c_single_byte_out; 	// Global holder for single byte writes.

unsigned char* p_i2c_data_out; 	// Global pointer for SMBus data. All transmit data is read from here

unsigned char i2c_data_len; 	// Global holder for number of bytes to send or receive in the current SMBus transfer.

unsigned char i2c_word_addr; 		// Global holder for the EEPROM word address that will be accessed in the next transfer

unsigned char i2c_target;       	// Target SMBus slave address

bit i2c_busy; 		// Software flag to indicate when the EEPROM_ByteRead() or EEPROM_ByteWrite() functions have claimed the SMBus

bit i2c_rw; 		// Software flag to indicate the direction of the current transfer

bit i2c_send_word_addr; 	// When set, this flag causes the ISR to send the 8-bit <word_addr> after sending the slave address.

bit i2c_random_read;  	// When set, this flag causes the ISR to send a START signal after sending the word address.
               			// For the 24LC02B EEPROM, a random read (a read from a particular address in memory) starts as a write then
                 		// changes to a read after the repeated start is sent. 
						// The ISR handles this switchover if the <i2c_random_read> bit is set.

bit i2c_ack_poll; 		// When set, this flag causes the ISR to send a repeated START until the slave has acknowledged its address


//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------

void i2c_Init(void);
void i2c_ISR(void);

void i2c_ByteWrite(unsigned char device_addr, unsigned char data_addr, unsigned char *dat, unsigned char num_byte);
void i2c_ByteRead(unsigned char device_addr, unsigned char data_addr, unsigned char *dat, unsigned char num_byte);

//-----------------------------------------------------------------------------
// Initialization Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// i2c_Init()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// The SMBus peripheral is configured as follows:
// - SMBus enabled
// - Slave mode disabled
// - Timer1 used as clock source. The maximum SCL frequency will be
//   approximately 1/3 the Timer1 overflow rate
// - Setup and hold time extensions enabled
// - Free and SCL low timeout detection enabled
//
void i2c_Init(void)
{  
	CKCON     |= 0x10;		// Timer2 clock source = SYSCLK
	TMR2CN    = 0x08;      	// Timer2 in 8-bit auto-reload mode
							// Timer2 enabled
	TMR2RLL   = 0xEC; 		// 100kHz                 

	SMB0CF    = 0x83;       // Enable SMBus; Use Timer2 High byte overflows as SMBus clock source; 
	EIE1      |= 0x01;
	i2c_status = I2C_FREE;
	i2c_busy = 0;
}



//-----------------------------------------------------------------------------
// SMBus Interrupt Service Routine (ISR)
//-----------------------------------------------------------------------------
//
// SMBus ISR state machine
// - Master only implementation - no slave or arbitration states defined
// - All incoming data is written starting at the global pointer <p_i2c_data_in>
// - All outgoing data is read from the global pointer <p_i2c_data_out>
//
void i2c_ISR (void) interrupt 7
{
	bit FAIL = 0;             		// Used by the ISR to flag failed transfers
	static char i;              	// Used by the ISR to count the number of data bytes sent or received
	static bit SEND_START = 0;    	// Send a start

	switch (SMB0CN & 0xF0)     	   	// Status vector
	{
		case I2C_MTSTA:    //0xE0	// Master Transmitter/Receiver: START condition transmitted.
			SMB0DAT = i2c_target;    	// Load address of the target slave
			SMB0DAT &= 0xFE;    	// Clear the LSB of the address for the
			// R/W bit
			SMB0DAT |= i2c_rw;   	// Load R/W bit
			STA = 0;              	// Manually clear START bit
			i = 0;                 	// Reset data byte counter
			break;

		case I2C_MTDB:     //0xC0	// Master Transmitter: Data byte (or Slave Address) transmitted
			if (ACK)               	// Slave Address or Data Byte Acknowledged?
			{                              
				if (SEND_START)
				{
					STA = 1;
					SEND_START = 0;
					break;
				}
				if(i2c_send_word_addr) 		// Are we sending the word address?
				{
					i2c_send_word_addr = 0;   // Clear flag
					SMB0DAT = i2c_word_addr;    // Send word address

					if (i2c_random_read)
					{
						SEND_START = 1;   	// Send a START after the next ACK cycle
						i2c_rw = I2C_READ;
					}

					break;
				}

				if (i2c_rw == I2C_WRITE)   	// Is this transfer a WRITE?
				{

					if (i < i2c_data_len)   // Is there data to send?
					{
						SMB0DAT = *p_i2c_data_out;	//*p_i2c_data_out;	// send data byte						
						p_i2c_data_out++;			// increment data out pointer
						i++;						// increment number of bytes sent
					}
					else
					{
						STO = 1;              	// Set STO to terminte transfer
						i2c_busy = 0;         	// Clear software busy flag
						i2c_status = I2C_FREE;
					}
				}
				else 
				{}		// If this transfer is a READ, then take no action. Slave address was transmitted.
						// A separate 'case' is defined for data byte received.
			}
			else 		// If slave NACK,
			{
				if(i2c_ack_poll)
				{
					STA = 1; 		// Restart transfer
				}
				else
				{
					FAIL = 1;      	// Indicate failed transfer and handle at end of ISR
				}                           
			}
			break;

		case I2C_MRDB:     //0x80	// Master Receiver: byte received
			if ( i < i2c_data_len )	// Is there any data remaining?
			{
				*p_i2c_data_in = SMB0DAT;	// Store received byte
				p_i2c_data_in++;            	// Increment data in pointer
				i++; 
				ACK = 1;                   	// Set ACK bit (may be cleared later in the code)

			}

			if (i == i2c_data_len)  		// This is the last byte
			{
				i2c_busy = 0;              	// Free SMBus interface
				ACK = 0;                   	//  NACK to indicate last byte of this transfer
				STO = 1;                   	// Send STOP to terminate transfer
				i2c_status = I2C_FREE;
				_nop_();
			}

			break;

		default:
			FAIL = 1;                     	// Indicate failed transfer and handle at end of ISR
			break;
	}

	if (FAIL)                           	// If the transfer failed,
	{
		SMB0CF &= ~0x80;                 	// Reset communication
		SMB0CF |= 0x80;
		STA = 0;
		STO = 0;
		ACK = 0;

		i2c_busy = 0;                    	// Free SMBus
		i2c_status = I2C_FREE;	
		FAIL = 0;
	}

	SI = 0;                             	// Clear interrupt flag
}


//-----------------------------------------------------------------------------
// Support Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// i2c_ByteWrite ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//   1) unsigned char addr - address to write in the EEPROM
//                        range is full range of character: 0 to 255
//
//   2) unsigned char dat - data to write to the address <addr> in the EEPROM
//                        range is full range of character: 0 to 255
//
// This function writes the value in <dat> to location <addr> in the EEPROM
// then polls the EEPROM until the write is complete.
//
void i2c_ByteWrite(unsigned char device_addr, unsigned char data_addr, unsigned char *dat, unsigned char num_byte)
{
	i2c_status = I2C_WRITE_EEPROM;

	// Set SMBus ISR parameters
	i2c_target = device_addr; 	// Set target slave address
	i2c_rw = I2C_WRITE;     // Mark next transfer as a write
	i2c_send_word_addr = 1;  	// Send Word Address after Slave Address
	i2c_random_read = 0;     // Do not send a START signal after the word address
	i2c_ack_poll = 1;      	// Enable Acknowledge Polling (The ISR will automatically restart the
						 	// transfer if the slave does not acknowledge its address.)

	// Specify the Outgoing Data
	if(num_byte == 1)
	{
		i2c_word_addr = data_addr & 0x7F; 	// Set the target address in the EEPROM's internal memory space
	}
	else if(num_byte > 1)
	{
		i2c_word_addr = (data_addr & 0x7F) | I2C_AUTO; 	// Set the target address in the EEPROM's internal memory space
	}
	
	p_i2c_data_out = dat;	// Store <dat> (local variable) in a global variable so the ISR can read
							 	// it after this function exits

	i2c_data_len = num_byte; 		// Specify to ISR that the next transfer will contain one data byte

	STA = 1;				// Initiate SMBus Transfer

}


//-----------------------------------------------------------------------------
// i2c_ByteRead ()
//-----------------------------------------------------------------------------
//
// Return Value :
//   1) unsigned char data - data read from address <addr> in the EEPROM
//                        range is full range of character: 0 to 255
//
// Parameters   :
//   1) unsigned char addr - address to read data from the EEPROM
//                        range is full range of character: 0 to 255
//
// This function returns a single byte from location <addr> in the EEPROM then
// polls the <i2c_busy> flag until the read is complete.
//
void i2c_ByteRead(unsigned char device_addr, unsigned char data_addr, unsigned char *dat, unsigned char num_byte)
{
	i2c_status = I2C_READ_EEPROM;

	// Set SMBus ISR parameters
	i2c_target = device_addr;  	// Set target slave address
	i2c_rw = I2C_WRITE;   	// A random read starts as a write then changes to a read after
							// the repeated start is sent. The ISR handles this switchover if
							// the <i2c_random_read> bit is set.
	i2c_send_word_addr = 1;  	// Send Word Address after Slave Address
	i2c_random_read = 1;   	// Send a START after the word address
	i2c_ack_poll = 1;      	// Enable Acknowledge Polling

	// Specify the Incoming Data
	if(num_byte == 1)
	{
		i2c_word_addr = data_addr & 0x7F; 	// Set the target address in the EEPROM's internal memory space
	}
	else if(num_byte > 1)
	{
		i2c_word_addr = (data_addr & 0x7F) | I2C_AUTO; 	// Set the target address in the EEPROM's internal memory space
	}

	p_i2c_data_in = dat;
	i2c_data_len = num_byte;  		// Specify to ISR that the next transfer will contain one data byte

	STA = 1;				// Initiate SMBus Transfer
}


//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------