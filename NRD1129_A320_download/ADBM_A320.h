
//;***************************************************************************
//;
//;        Copyright 2007 Avago Technologies Limited
//;    This code is provided by Avago Technologies as a reference.
//;     Avago Technologies makes no claims or warranties
//;    to this firmware's suitability for any application.
//;    All rights reserved. Patent Pending. 
//;
//;*************************************************************************** 


// ************************** DATA TYPES & DEFINITIONS **************************

#define ADBM_A320_POWERUP_RETRIES  		 10

// Configuration Register Individual Bit Field Settings
#define ADBM_A320_MOTION_MOT       		 0x80
#define ADBM_A320_MOTION_PIXRDY    		 0x40
#define ADBM_A320_MOTION_PIXFIRST  		 0x20
#define ADBM_A320_MOTION_OVF       		 0x10
#define ADBM_A320_MOTION_GPIO      		 0x01

// Configuration Register Settings
#define ADBM_A320_SOFTRESET_INIT  		 0x5A
#define ADBM_A320_SELF_TEST     		 0x01
#define ADBM_A320_NO_REST              	 0x00
#define ADBM_A320_REST1              	 0x10
#define ADBM_A320_REST2              	 0x20
#define ADBM_A320_REST3              	 0x30
#define ADBM_A320_RES500CPI          	 0x00
#define ADBM_A320_RES1000CPI          	 0x80
#define ADBM_A320_LED0               	 0x01     //1:LED continuous ON
#define ADBM_A320_LED3             	 	 0x08     //0:13mA; 1:20mA
#define ADBM_A320_BURST               	 0x10
#define ADBM_A320_SPI               	 0x04
#define ADBM_A320_TWI              	 	 0x01
#define ADBM_A320_RUN_MODE          	 0x00
#define ADBM_A320_REST1_MODE          	 0x40
#define ADBM_A320_REST2_MODE          	 0x80
#define ADBM_A320_REST3_MODE          	 0xC0

// ADBM_A320 Register Addresses
#define ADBM_A320_PRODUCTID_ADDR         0x00     //0x83
#define ADBM_A320_REVISIONID_ADDR        0x01     //0x00
#define ADBM_A320_MOTION_ADDR            0x02
#define ADBM_A320_DELTAX_ADDR            0x03
#define ADBM_A320_DELTAY_ADDR            0x04
#define ADBM_A320_SQUAL_ADDR             0x05
#define ADBM_A320_SHUTTERUPPER_ADDR      0x06
#define ADBM_A320_SHUTTERLOWER_ADDR      0x07
#define ADBM_A320_MAXIMUMPIXEL_ADDR      0x08
#define ADBM_A320_PIXELSUM_ADDR          0x09
#define ADBM_A320_MINIMUMPIXEL_ADDR      0x0A
#define ADBM_A320_PIXELGRAB_ADDR         0x0B
#define ADBM_A320_CRC0_ADDR              0x0C
#define ADBM_A320_CRC1_ADDR              0x0D
#define ADBM_A320_CRC2_ADDR              0x0E
#define ADBM_A320_CRC3_ADDR              0x0F
#define ADBM_A320_SELFTEST_ADDR          0x10
#define ADBM_A320_CONFIGURATIONBITS_ADDR 0x11
#define ADBM_A320_LED_CONTROL_ADDR       0x1A
#define ADBM_A320_IO_MODE_ADDR           0x1C
#define ADBM_A320_OBSERVATION_ADDR       0x2E
#define ADBM_A320_SOFTRESET_ADDR      	 0x3A     //0x5A
#define ADBM_A320_SHUTTER_MAX_HI_ADDR    0x3B
#define ADBM_A320_SHUTTER_MAX_LO_ADDR    0x3C
#define ADBM_A320_INVERSEREVISIONID_ADDR 0x3E     //0xFF
#define ADBM_A320_INVERSEPRODUCTID_ADDR  0x3F     //0x7C

#define ADBM_A320_OFN_ENGINE_ADDR  				0x60
#define ADBM_A320_OFN_RESOLUTION_ADDR  			0x62
#define ADBM_A320_OFN_SPEED_CONTROL_ADDR  		0x63
#define ADBM_A320_OFN_SPEED_ST12_ADDR  			0x64
#define ADBM_A320_OFN_SPEED_ST21_ADDR  			0x65
#define ADBM_A320_OFN_SPEED_ST23_ADDR  			0x66
#define ADBM_A320_OFN_SPEED_ST32_ADDR  			0x67
#define ADBM_A320_OFN_SPEED_ST34_ADDR  			0x68
#define ADBM_A320_OFN_SPEED_ST43_ADDR  			0x69
#define ADBM_A320_OFN_SPEED_ST45_ADDR  			0x6A
#define ADBM_A320_OFN_SPEED_ST54_ADDR  			0x6B
#define ADBM_A320_OFN_AD_CTRL_ADDR  			0x6D
#define ADBM_A320_OFN_AD_ATH_HIGH_ADDR  		0x6E
#define ADBM_A320_OFN_AD_DTH_HIGH_ADDR  		0x6F
#define ADBM_A320_OFN_AD_ATH_LOW_ADDR  			0x70
#define ADBM_A320_OFN_AD_DTH_LOW_ADDR  			0x71
#define ADBM_A320_OFN_QUANTIZE_CTRL_ADDR  		0x73
#define ADBM_A320_OFN_XYQ_THRESH_ADDR  			0x74
#define ADBM_A320_OFN_FPD_CTRL_ADDR  			0x75
#define ADBM_A320_OFN_ORIENTATION_CTRL_ADDR  	0x77

//------------------------------------------------------------

