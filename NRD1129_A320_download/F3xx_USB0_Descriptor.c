//-----------------------------------------------------------------------------
// F3xx_USB0_Descriptor.c
//-----------------------------------------------------------------------------
// Copyright 2005 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Source file for USB firmware. Includes descriptor data.
//
//
// How To Test:    See Readme.txt
//
//
// FID:            3XX000031
// Target:         C8051F32x/C8051F340
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
//                 Silicon Laboratories IDE version 2.6
// Command Line:   See Readme.txt
// Project Name:   F3xx_MouseExample
//
//
// Release 1.1
//    -Minor code comment changes
//    -16 NOV 2006
// Release 1.0
//    -Initial Revision (PD)
//    -07 DEC 2005
//
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <c8051f3xx.h>	// SFR declarations

#include "F3xx_USB0_Register.h"
#include "F3xx_USB0_InterruptServiceRoutine.h"
#include "F3xx_USB0_Descriptor.h"

//-----------------------------------------------------------------------------
// Descriptor Declarations
//-----------------------------------------------------------------------------

#define LE(x)	((((x)&0x00FF)<<8)|(((x)&0xFF00)>>8))	// convert to little endian
#define PIXEL_NUM_MSB 	(unsigned char)(((PIXEL_NUMBER + 1) & 0xFF00)>>8) //msb of pixel number
#define PIXEL_NUM_LSB 	(unsigned char) (PIXEL_NUMBER + 1) //lsb of pixel number

code const device_descriptor DEVICEDESC =
{
   18,                                 // bLength
   0x01,                               // bDescriptorType
   LE(0x0200),                         // bcdUSB
   0x00,                               // bDeviceClass
   0x00,                               // bDeviceSubClass
   0x00,                               // bDeviceProtocol
   EP0_PACKET_SIZE,                    // bMaxPacketSize0
   LE(0x192F),                         // idVendor
   LE(0x0000),                         // idProduct
   LE(0x0000),                         // bcdDevice
   0x01,                               // iManufacturer
   0x02,                               // iProduct
   0x00,                               // iSerialNumber
   0x01                                // bNumConfigurations
}; //end of DEVICEDESC

// From "USB Device Class Definition for Human Interface Devices (HID)".
// Section 7.1:
// "When a Get_Descriptor(Configuration) request is issued,
// it returns the Configuration descriptor, all Interface descriptors,
// all Endpoint descriptors, and the HID descriptor for each interface."
code const hid_configuration_descriptor HIDCONFIGDESC =
{

{ // configuration_descriptor hid_configuration_descriptor
   0x09,                               // Length
   0x02,                               // Type
   LE(sizeof(hid_configuration_descriptor)),  // wTotalLength
   0x01,                               // NumInterfaces
   0x01,                               // bConfigurationValue
   0x00,                               // iConfiguration
   0x80,                               // bmAttributes
   0x20                                // MaxPower (in 2mA units)
},

{ // interface_descriptor hid_interface_descriptor
   0x09,                               // bLength
   0x04,                               // bDescriptorType
   0x00,                               // bInterfaceNumber
   0x00,                               // bAlternateSetting
   0x01,                               // bNumEndpoints
   0x03,                               // bInterfaceClass (3 = HID)
   0x01,                               // bInterfaceSubClass
   0x02,                               // bInterfaceProcotol
   0x00                                // iInterface
},

{ // class_descriptor hid_descriptor
	0x09,	                           // bLength
	0x21,	                           // bDescriptorType
	LE(0x0101),
	0x00,	                           // bCountryCode
	0x01,	                           // bNumDescriptors
	0x22,                              // bDescriptorType
	LE(sizeof(hid_report_descriptor))  // wItemLength (tot. len. of report
                                       // descriptor)
},

// IN EP1 endpoint (mandatory for HID)
{ // endpoint_descriptor hid_endpoint_in_descriptor
   0x07,                               // bLength
   0x05,                               // bDescriptorType
   0x81,                               // bEndpointAddress
   0x03,                               // bmAttributes
   LE(EP1_PACKET_SIZE),                // wMaxPacketSize (LITTLE ENDIAN)
   1                                   // bInterval
},

// OUT EP1 endpoint (optional for HID)
{ // endpoint_descriptor hid_endpoint_out_descriptor
   0x07,                               // bLength
   0x05,                               // bDescriptorType
   0x01,                               // bEndpointAddress
   0x03,                               // bmAttributes
   LE(EP1_PACKET_SIZE),                // wMaxPacketSize (LITTLE ENDIAN)
   1                                  // bInterval
}

};


code const hid_report_descriptor HIDREPORTDESC =
{
    0x05, 0x01,                        // Usage Page (Generic Desktop)
    0x09, 0x02,                        // Usage (Mouse)
    0xA1, 0x01,                        // Collection (Application)
	0x85, 0x01,       			   	   //   REPORT ID (0x01)
    0x09, 0x01,                        //   Usage (Pointer)
    0xA1, 0x00,                        //   Collection (Physical)
    0x05, 0x09,                        //     Usage Page (Buttons)
    0x19, 0x01,                        //     Usage Minimum (1)
    0x29, 0x03,                        //     Usage Maximum (3)
    0x15, 0x00,                        //     Logical Minimum (0)
    0x25, 0x01,                        //     Logical Maximum (1)
    0x95, 0x03,                        //     Report Count (1)
    0x75, 0x01,                        //     Report Size (1)
    0x81, 0x02,                        //     Input (Data, Variable, Absolute)
    0x95, 0x01,                        //     Report Count (1)
    0x75, 0x05,                        //     Report Size (5)
    0x81, 0x01,                        //     Input (Constant) for padding
    0x05, 0x01,                        //     Usage Page (Generic Desktop)
    0x09, 0x30,                        //     Usage (X)
    0x09, 0x31,                        //     Usage (Y)
    0x09, 0x38,                    	   //     USAGE (Wheel)
    0x15, 0x81,                        //     Logical Minimum (-127)
    0x25, 0x7F,                        //     Logical Maximum (127)
    0x75, 0x08,                        //     Report Size (8)
    0x95, 0x03,                        //     Report Count (3)
    0x81, 0x06,                        //     Input (Data, Variable, Relative)

	0x06, 0x00, 0xff,              //     USAGE_PAGE (Vendor Defined Page 1)
    0x09, 0x01,                    //     USAGE (Vendor Usage 1)
    0x15, 0x80,                    //     LOGICAL_MINIMUM (-128)
    0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,                    //     REPORT_SIZE (8)
 	0x95, 0x03,                    //     REPORT_COUNT (3)
    0xb1, 0x02,                    //     FEATURE (Data,Var,Abs)

    0xC0,                              //   End Collection (Physical)
    0xC0,                               // End Collection (Application)

	0x06, 0x00, 0xff,              // USAGE_PAGE (Vendor Defined Page 1)
    0x09, 0x01,                    // USAGE (Vendor Usage 1)
	0xa1, 0x01,                    // COLLECTION (Application)
	0x85, 0x02,       			   //   REPORT ID (0x02)
	0xa1, 0x00,                    //   COLLECTION (Physical)
	0x09, 0x02,                    //   USAGE (Vendor Usage 1)	
    0x15, 0x80,                    //   LOGICAL_MINIMUM (-128)
    0x25, 0x7f,                    //   LOGICAL_MAXIMUM (127)
    0x75, 0x08,                    //   REPORT_SIZE (8)
	0x96, PIXEL_NUM_LSB, PIXEL_NUM_MSB,	//   REPORT_COUNT (451)
    0xb1, 0x02,                    //   FEATURE (Data,Var,Abs)
	0xc0,                          //	END_COLLECTION
    0xc0,                          // END_COLLECTION

	0x05, 0x01,                    	// USAGE_PAGE (Generic Desktop)
    0x09, 0x06,                    	// USAGE (Keyboard)
    0xa1, 0x01,                    	// COLLECTION (Application)
    0x85, 0x03,                    	//   REPORT_ID (3)
    0x05, 0x07,                    	//   USAGE_PAGE (Keyboard)
    0x19, 0xe0,                    	//   USAGE_MINIMUM (Keyboard LeftControl)
    0x29, 0xe7,                    	//   USAGE_MAXIMUM (Keyboard Right GUI)
    0x15, 0x00,                    	//   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    	//   LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    	//   REPORT_SIZE (1)
    
	0x95, 0x08,                    	//   REPORT_COUNT (8)
    0x81, 0x02,                    	//   INPUT (Data,Var,Abs)
    0x95, 0x01,                    	//   REPORT_COUNT (1)
    0x75, 0x08,                    	//   REPORT_SIZE (8)
    0x81, 0x01,                    	//   INPUT (Cnst,Ary,Abs)
    0x95, 0x05,                    	//   REPORT_COUNT (5)
    0x75, 0x01,                    	//   REPORT_SIZE (1)
    0x05, 0x08,                    	//   USAGE_PAGE (LEDs)
    0x19, 0x01,                    	//   USAGE_MINIMUM (Num Lock)
    0x29, 0x05,                    	//   USAGE_MAXIMUM (Kana)
    
	0x91, 0x02,                    	//   OUTPUT (Data,Var,Abs)
    0x95, 0x01,                    	//   REPORT_COUNT (1)
    0x75, 0x03,                    	//   REPORT_SIZE (3)
    0x91, 0x01,                    	//   OUTPUT (Cnst,Ary,Abs)
    0x95, 0x06,                    	//   REPORT_COUNT (6)
    0x75, 0x08,                    	//   REPORT_SIZE (8)
    0x15, 0x00,                    	//   LOGICAL_MINIMUM (0)
    0x25, 0x65,                    	//   LOGICAL_MAXIMUM (101)
    0x05, 0x07,                    	//   USAGE_PAGE (Keyboard)
    0x19, 0x00,                    	//   USAGE_MINIMUM (Reserved (no event indicated))
    
	0x29, 0x65,                    	//   USAGE_MAXIMUM (Keyboard Application)
    0x81, 0x00,                    	//   INPUT (Data,Ary,Abs)
    0xC0,							// END_COLLECTION

		0x05, 0x01,                        // Usage Page (Generic Desktop)
    0x09, 0x04,                        // Usage (Joystick)
    0xA1, 0x01,                        // Collection (Application)
	0x85, 0x04,       			   	   //   REPORT ID (0x04)
	
    0x09, 0x01,                        //   Usage (Pointer)
    0xA1, 0x00,                        //   Collection (Physical)
    0x05, 0x01,                        //     Usage Page (Generic Desktop)
    0x09, 0x30,                        //     Usage (X)
    0x09, 0x31,                        //     Usage (Y)
    0x15, 0x00,                  	   //     Logical Minimum (0)
    0x26, 0xFF, 0x00,                  //     Logical Maximum (255)
    0x35, 0x00,                  	   //     Physical Minimum (0)
    0x46, 0xFF, 0x00,                  //     Physical Maximum (255)
    0x65, 0x00,						   //	  Unit (None)
    0x75, 0x08,                        //     Report Size (8)
    0x95, 0x02,                        //     Report Count (2)
    0x81, 0x02,                        //     Input (Data, Variable, Absolute)
    
    0x05, 0x09,                        //     Usage Page (Button)
    0x15, 0x00,                        //     Logical Minimum (0)
    0x25, 0x01,                        //     Logical Maximum (1)
    0x19, 0x01,                        //     Usage Minimum (1)
    0x29, 0x08,                        //     Usage Maximum (8)
    0x95, 0x08,                        //     Report Count (8)
    0x75, 0x01,                        //     Report Size (1)
    0x81, 0x02,                        //     Input (Data, Variable, Absolute)

    0xC0,                              //   End Collection (Physical)
    0xC0,                               // End Collection (Application)

};


#define STR0LEN 4

code const unsigned char String0Desc [STR0LEN] =
{
   STR0LEN, 0x03, 0x09, 0x04
}; //end of String0Desc

#define STR1LEN sizeof ("AVAGO TECHNOLOGIES") * 2

code const unsigned char String1Desc [STR1LEN] =
{
   STR1LEN, 0x03,
   'A', 0,
   'V', 0,
   'A', 0,
   'G', 0,
   'O', 0,
   ' ', 0,
   'T', 0,
   'E', 0,
   'C', 0,
   'H', 0,
   'N', 0,
   'O', 0,
   'L', 0,
   'O', 0,
   'G', 0,
   'I', 0,
   'E', 0,
   'S', 0
}; //end of String1Desc

#define STR2LEN sizeof ("AVAGO USB OFN Device") * 2

code const unsigned char String2Desc [STR2LEN] =
{
   STR2LEN, 0x03,
   'A', 0,
   'V', 0,
   'A', 0,
   'G', 0,
   'O', 0,
   ' ', 0,
   'U', 0,
   'S', 0,
   'B', 0,
   ' ', 0,
   'O', 0,
   'F', 0,
   'N', 0,
   ' ', 0,
   'D', 0,
   'e', 0,
   'v', 0,
   'i', 0,
   'c', 0,
   'e', 0,
}; //end of String2Desc

unsigned char* const STRINGDESCTABLE [] =
{
   String0Desc,
   String1Desc,
   String2Desc
};