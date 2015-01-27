//;***************************************************************************
//;
//;        Copyright 2007 Avago Technologies Limited
//;    This code is provided by Avago Technologies as a reference.
//;     Avago Technologies makes no claims or warranties
//;    to this firmware's suitabilityor any application.
//;    All rights reserved. Patent Pending. 
//;
//;*************************************************************************** 

//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include <c8051f3xx.h>	// SFR declarations
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <intrins.h>
#include <timer.h>
#include <SPI_TRANSFER.h>
#include <F3xx_USB0_Mouse.h>
#include <F3xx_USB0_ReportHandler.h>
#include <F3xx_USB0_InterruptServiceRoutine.h>
#include <i2c.h>
#include <rocker.h>





	////////////////////////////////////////////////////////////////////////////
	// User Code Here: Add business logic 
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	// End User Code 
	////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------
// CONSTANT
//------------------------------------------------------------------------------------

//#define JOYSTICK

#define DEBOUNCE_TIME		5

#define MOUSE_MODE			1
#define MOUSE_DATA_LENGTH	5
#define IMAGE_MODE			2
#define IMAGE_DATA_LENGTH	5
#define KEY_MODE			3
#define KEY_DATA_LENGTH		9

/// Modified by TLLAU BEGIN ///
#define JOYSTICK_USB_MODE		4
#define JOYSTICK_DATA_LENGTH	9				
/// Modified by TLLAU END ///


#define SENSOR_TIME 	8

#define UP				0
#define DOWN			1
#define RIGHT			2
#define LEFT			3

//#define SCROLL_MODE		10
#define ROCKER_M_MODE	12
#define ROCKER_K_MODE	13	

#define FINGER_MODE			0
#define SCROLL_MODE			3	// 5. Modified by TLLAU			
#define ROCKER_K_MODE_S		6
#define ROCKER_K_MODE_L		2	// 3. Modified by TLLAU
#define ROCKER_K_MODE_S_C	1
#define ROCKER_K_MODE_L_C	7	// 2. Modified by TLLAU
#define ROCKER_K_MODE_L_8	5	// Modified by TLLAU

/// Modified by TLLAU BEGIN ///
#define JOYSTICK_MODE		4

#define MODE_NUM			5	

#define JOYSTICK_DELAY		100
#define JOYSTICK_CENTER		127

#define UP_R_KEY		0x01
#define UP_L_KEY		0x02
#define DOWN_R_KEY		0x03
#define DOWN_L_KEY		0x04
/// Modified by TLLAU END ///

#define TAP_TIME		1000
#define STEP_DELAY		100
#define SHUTTER_DELAY	0x0003
#define MOUSE_STEP		20

#define TWENTY_MA		20
#define TEN_MA			10

//------------------------------------------------------------------------------------
// Global VARIABLES
//------------------------------------------------------------------------------------
typedef union
{
	unsigned int w;
	unsigned char b[2];
}
union16;

unsigned char data seq0_state, seq1_state, seq3_state, seq4_state;
unsigned char data seq5_state, seq6_state, seq11_state;
unsigned char data seq13_state;

unsigned char data seq7_state;

//FPD fix
int temp_a;
unsigned char finger_leave_counter1,finger_leave_counter2,finger_on_counter;
unsigned char motion0, motion1, motion2;

bit L7; // Modified by TLLAU

// communication with PC
union16 para1, para2;
unsigned int image_index;
xdata unsigned char buffer[20];
xdata unsigned char image_buffer[PIXEL_NUMBER + 2];
signed char count_x_temp, count_y_temp;
unsigned int index;
bit start_flag;

// mouse packet
unsigned int num_tap, num_tap_count;
signed char count_x, count_y, count_z;
signed  char mod_x, mod_y, mod_z;
unsigned char sent_buttons;

// button
unsigned char buttons;
//bdata unsigned char buttons;
//sbit left_button	= buttons^0;
//sbit right_button	= buttons^1;
//sbit middle_button	= buttons^2;


// motion burst
signed char delta_y_hist[100];
unsigned char hist_counter;
unsigned char motion;
int delta_x, delta_y;
unsigned char data squal, shutter_h, shutter_l;
//unsigned char data pix_min, pix_max;

// LED
unsigned char led_flag, duty_cycle;
union16 pca_timer, pca_counter;

// mode 
unsigned char usb_mode, ofn_mode, scroll_mode, free_scroll, communication_mode;
unsigned char led_temp;

// speed switching
unsigned int data shutter_flag, shutter_flag_temp;
unsigned int count_x_temp1, count_x_temp2, count_x_temp3;
signed int average_shutter_w_motion;
signed char count_x0, count_x1, count_x2, count_x3, count_x4;	
signed char count_y0, count_y1, count_y2, count_y3, count_y4;
union16 shutter0, shutter1, shutter6;

// flag / temp
unsigned char temp;
unsigned char x_temp, y_temp, z_temp;
bit shut_flag, chk_usb;
extern unsigned char spi_temp;
extern unsigned char click_flag;
extern bit rc_flag, sc_flag;
unsigned char pix_min, pix_max, pix_avg;
unsigned char FPD_state;
unsigned char history_motion,history_motion_counter;

// shutter calibration
extern unsigned int shutter_basic, shutter_d;
extern unsigned int SHUTTER_UPPER_TRES, SHUTTER_LOWER_TRES;
//extern unsigned char data PIX_MIN_CALIB;
extern bit threshold_flag;


// toggle step
unsigned char keyboard_dir, keyboard_dir_temp;
unsigned char keyboard_tap, keyboard_tap1;
unsigned char click_flag_temp, keyboard_tap_temp;
unsigned char keyboard_temp;
bit tap_flag;
extern unsigned int shutter_motion1, shutter_motion2;
signed char sign_count_x0, sign_count_x1, sign_count_x2, sign_count_x3, sign_count_x4;	
signed char sign_count_y0, sign_count_y1, sign_count_y2, sign_count_y3, sign_count_y4;
signed int X_Total_Displacement_main, Y_Total_Displacement_main; 


// state
extern unsigned char left_state, right_state, click_state, calibrate_state;

// rocker
signed char x_raw, y_raw;
unsigned char k_raw;
int rocker_delta_x, rocker_delta_y, continuous_delta_x, continuous_delta_y, rocker_delta_x_T, rocker_delta_y_T;
unsigned char send_rocker_data, start_stepping, start_continuous_stepping, issued_times = 0;


//LED ratio FPD
//unsigned char last_state, written, perform_check_FPD,shutter_values_captured;
//int current_shutter, previous_shutter, shutter_ratio;
//unsigned char FPD_turned_off, led_switching_start;


//------------------------------------------------------------------------------------
// VARIABLES
//------------------------------------------------------------------------------------
extern data unsigned char IN_PACKET[10];
data unsigned char OUT_PACKET[5];
data unsigned char grab_image;
data unsigned char received;
unsigned char motion_buffer[10];


int multiple_step_rocker_flag;

//------------------------------------------------------------------------------------
// NOKIA PARAMETERS
//------------------------------------------------------------------------------------
//ACCELERATION
int n1, n2, n3, n4;
int t1, t2, t3, t4, t5; 

n1 = 2; 
n2 = 10; 
n3 = 26; 
n4 = 58;
t1 = 500; 
t2 = 250; 
t3 = 125; 
t4 = 62; 
t5 = 31; 

//START| Speed 1 | Speed 2      | Speed 3      | Speed 4      | Speed 5
//START| n1 x t1 | (n2-n1) x t2 | (n3-n2) x t3 | (n4-n3) x t4 | Polling rate based on t5
//
//Default Setting:
//START| Speed 1 | Speed 2      | Speed 3      | Speed 4      | Speed 5
//START| 2 x 500 | (10-8) x 250 | (26-10)x 125 | (58-26) x 62 | Polling rate of 31ms  
//	   | 1000    | 2000 		| 2000         |  1984        | beyond 'Speed 4'
//
//n=repetition
//t=polling rate(ms)
 

//TRACKING ON/OFF - Disable/Enable Navigation ( 1 = ON, 0 = OFF)
int TrackSW = 1;

//COUNTS PER SWIPE
extern int step_multiplier_mode1 = 1;	//affects the sensitivity in mouse mode (use integers between 1-5)
//extern int step_multiplier_mode2 = 1;
//extern int step_multiplier_mode3 = 1;
extern int step_multiplier_mode4 = 1;
extern int step_multiplier_mode5 = 1;




//------------------------------------------------------------------------------------
// Function PROTOTYPES
//------------------------------------------------------------------------------------
void Image_Dump(void);
void Left_Switch(void);
void Right_Switch(void);
void Mode_Select_Switch(void);
void Motion_SPInTWI (void);
void USB_Send_Data(void);
void Motion_TWI(void);

void Dome_Switch(void);
void LED_driving_current_switch (void);

unsigned char toggle_step(int X_Total_Displacement, int Y_Total_Displacement);
unsigned char toggle_step2(int deltaX, int deltaY);
int X_Acceleration_Buffer, Y_Acceleration_Buffer;
void data_process(void);
unsigned char key_step, key_step2, finger_status; // change

/// Modified by TLLAU BEGIN ///
#include <math.h>

#define PI					3.141592f
#define SHUTTER_THERS_UP	1200
#define SHUTTER_THERS_LO	500
#define KEYPRESS_DELAY		100

unsigned char joystick_dir(signed char deltaX, signed char deltaY);
unsigned char keyboard_tap_repeat;
extern bit toggle_flag;
extern signed int X_Total_Displacement, Y_Total_Displacement; 				
/// Modified by TLLAU END ///

//------------------------------------------------------------------------------------
// External Function PROTOTYPES
//------------------------------------------------------------------------------------
extern void SendPacket(void);
	////////////////////////////////////////////////////////////////////////////
	// User Code Here: Add business logic 
	////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////
	// End User Code 
	////////////////////////////////////////////////////////////////////////////


//------------------------------------------------------------------------------------
// InitSeq
//------------------------------------------------------------------------------------

//Initializes and sets the default values of all variables
void Sequence_Init(void)
{
	communication_mode = SPI_MODE;//SPI_MODE;

	Timer0Init();

	EA = 1;
	t0_data = 100;
	time_0 = 1;
	while(time_0 == 1);
	EA = 0;	

	EA = 1;

	if(communication_mode == I2C_MODE)
	{
	//	L6 = 1;
		Initiliaze_A320_I2C();
	}
	else if(communication_mode == SPI_MODE)
	{
	//	L6 = 0;
		Initiliaze_A320();
	}

	USB0_Init();
	
	left_state = 0;
	right_state = 0;
	click_state = 0;
	calibrate_state = 0;

	seq0_state = 0;
	seq1_state = 0;
	seq3_state = 0;
	seq4_state = 0;
	seq5_state = 0;
	seq6_state = 0;
	seq7_state = 0;
	seq11_state = 0;
	seq13_state = 0;

	threshold_flag = 0;
	index = 0;

	buffer_clear();

	scroll_mode = 0;
	free_scroll = 0;
	count_z = 0;
	shutter_flag = 0;
	buttons = 0x00;
	click_flag = 0;
	rc_flag = 0;
	sc_flag = 0;
	SHUTTER_UPPER_TRES = 0;
	SHUTTER_LOWER_TRES = 0;

	count_x = 0;
	count_y = 0;
	count_x3 = 0;
	count_y3 = 0;
	count_x4 = 0;
	count_y4 = 0;
	count_x_temp = 0;
	start_flag = 0;
	count_x_temp1 = 0;
	count_x_temp2 = 0;
	count_x_temp3 = 0;

	ofn_mode = FINGER_MODE;
	usb_mode = 1;
	shutter_flag_temp = SHUTTER_DELAY;

	x_raw = 0;
	y_raw = 0;
	k_raw = 0;

//	L1 = 0;	// Modified by TLLAU

	L2 = 0;	//cwl
	L3 = 1;
	L4 = 1;
	L5 = 1;
	
	L6 = 1;	// 0; Modified by TLLAU
	L7 = 1;
	LED7 = 1; // Modified by TLLAU

	if(communication_mode == I2C_MODE)
	{
		led_temp = ~P1;
		P1 &= ~0xFC;
		P1 |= (led_temp & 0xFC);
	}

	history_motion_counter = 0;

	threshold_flag = 1;
	num_tap = 0;
	num_tap_count = 0;
	hist_counter = 0;

	average_shutter_w_motion = 80;

	rocker_delta_x=0;
	rocker_delta_y=0;
	start_continuous_stepping = 0;


}




//------------------------------------------------------------------------------------
// Main Sequence
//------------------------------------------------------------------------------------
void usbTaskLoop(void)
{		
	Sequence_Init();
	while(1)
	{
		Image_Dump();					// Image Dump

		Left_Switch();					// Left Switch
		Right_Switch();					// Right Switch
		Mode_Select_Switch();			// Mode Select Switch
		Dome_Switch();					// Dome
		
		if(communication_mode == SPI_MODE)
		{
			Motion_SPInTWI ();				// Motion (SPI)
		}
		else if(communication_mode == I2C_MODE)
		{
			Motion_SPInTWI();				// Motion (SPI)
		}

		USB_Send_Data();				// USB sending data	
		
//		L6=!GPIO; Modified by TLLAU		
//CWL 090109		L1 = !GPIO; // Modified by TLLAU
		
	}
}




//------------------------------------------------------------------------------------
// Image_Dump (Image Dump)
//------------------------------------------------------------------------------------

void Image_Dump(void)
{
	switch(seq1_state)
	{
		case 0:
			if(grab_image == 1)
			{
				image_buffer[0] = 2; 	// report ID
				image_buffer[1] = 0;	// image no ready
				image_index = 2;
				grab_image = 0;
				chk_usb = 1;
				seq1_state = 30;
			}
			break;
		
		case 30:
			if(communication_mode == SPI_MODE)
			{
				EA = 0;
				spi_temp = spi_read_sensor(ADBM_A320_PIXELGRAB_ADDR);
				EA = 1;
				seq1_state = 40;
			}
			else if(communication_mode == I2C_MODE)
			{
				seq1_state = 32;
			}
			
			break;	
		case 32:
			if(i2c_status == I2C_FREE)
			{
				i2c_ByteRead(I2C_ADDR, ADBM_A320_PIXELGRAB_ADDR, &i2c_buffer, 1);
				seq1_state = 34;
			}
			break;	
		case 34:
			if(i2c_status == I2C_FREE)
			{
				spi_temp = i2c_buffer[0];
				seq1_state = 40;
			}
			break;

		case 40:
			if(communication_mode == SPI_MODE)
			{
				EA = 0;
				spi_write_sensor(ADBM_A320_PIXELGRAB_ADDR, 0x00);
				EA = 1;
				seq1_state = 50;
			}
			else if(communication_mode == I2C_MODE)
			{
				seq1_state = 42;
			}
			
			break;	
		case 42:
			if(i2c_status == I2C_FREE)
			{
				i2c_ByteWrite(I2C_ADDR, ADBM_A320_PIXELGRAB_ADDR, 0x00, 1);  
				seq1_state = 50;
			}
			break;

		case 50:
			if(communication_mode == SPI_MODE)
			{
				EA = 0;
				spi_temp = spi_read_sensor(ADBM_A320_MOTION_ADDR);
				EA = 1;
				if((spi_temp & (ADBM_A320_MOTION_PIXFIRST | ADBM_A320_MOTION_PIXRDY))
				== (ADBM_A320_MOTION_PIXFIRST | ADBM_A320_MOTION_PIXRDY))
				{
					seq1_state = 70;
				}
			}
			else if(communication_mode == I2C_MODE)
			{
				seq1_state = 52;
			}
			break;
		case 52:
			if(i2c_status == I2C_FREE)
			{
				i2c_ByteRead(I2C_ADDR, ADBM_A320_MOTION_ADDR, &i2c_buffer, 1);
				seq1_state = 54;
			}
			break;	
		case 54:
			if(i2c_status == I2C_FREE)
			{
				spi_temp = i2c_buffer[0];
				if((spi_temp & (ADBM_A320_MOTION_PIXFIRST | ADBM_A320_MOTION_PIXRDY))
				== (ADBM_A320_MOTION_PIXFIRST | ADBM_A320_MOTION_PIXRDY))
				{
					seq1_state = 70;
				}
				else
				{
					seq1_state = 52;
				}
			}
			break;	  
			   
		case 60:
			if(communication_mode == SPI_MODE)
			{
				EA = 0;
				spi_temp = spi_read_sensor(ADBM_A320_MOTION_ADDR);
				EA = 1;
				if((spi_temp & ADBM_A320_MOTION_PIXRDY) == ADBM_A320_MOTION_PIXRDY)
				{
					seq1_state = 70;
				}
			}
			else if(communication_mode == I2C_MODE)
			{
				seq1_state = 62;
			}
			break;
		case 62:
			if(i2c_status == I2C_FREE)
			{
				i2c_ByteRead(I2C_ADDR, ADBM_A320_MOTION_ADDR, &i2c_buffer, 1);
				seq1_state = 64;
			}
			break;	
		case 64:
			if(i2c_status == I2C_FREE)
			{
				spi_temp = i2c_buffer[0];
				if((spi_temp & ADBM_A320_MOTION_PIXRDY) == ADBM_A320_MOTION_PIXRDY)
				{
					seq1_state = 70;
				}
				else
				{
					seq1_state = 62;
				}
			}
			break;	 
				       
		case 70:
			if(communication_mode == SPI_MODE)
			{
				EA = 0;
				image_buffer[image_index++] = spi_read_sensor(ADBM_A320_PIXELGRAB_ADDR);
				EA = 1;
				seq1_state = 80;
			}
			else if(communication_mode == I2C_MODE)
			{
				seq1_state = 72;
			}
			break;
		case 72:
			if(i2c_status == I2C_FREE)
			{
				i2c_ByteRead(I2C_ADDR, ADBM_A320_PIXELGRAB_ADDR, &i2c_buffer, 1);
				seq1_state = 74;
			}
			break;	
		case 74:
			if(i2c_status == I2C_FREE)
			{
				image_buffer[image_index++] = i2c_buffer[0];
				seq1_state = 80;
			}
			break;
		
		case 80:
			if(chk_usb == 1)
			{
				if(image_index >= (PIXEL_NUMBER + 2))
				{
					image_index = 0;
					chk_usb = 0;
					image_buffer[1] = 1;		// image ready
					seq1_state = 0;
				}
				else
				{
					seq1_state = 60;
				}
			}
			break;		   
		
		default:
			break;
	}
}




//------------------------------------------------------------------------------------
// Left_Switch (Left Switch)
//------------------------------------------------------------------------------------

void Left_Switch(void)
{
	switch(seq3_state)
	{
		case 0:
			if(LEFT_SW == 0)
			{
				t3_data = DEBOUNCE_TIME;	//5
				time_3 = 1;
				seq3_state = 10;
			}
			break;
		case 10:
			if(LEFT_SW == 1)
			{
				seq3_state = 0;
			}
			else if(time_3 == 0)
			{
				free_scroll = 0;
				buffer_clear();
				count_z = 0;

				#ifdef JOYSTICK
					if((ofn_mode != JOYSTICK_MODE) || (ofn_mode != ROCKER_K_MODE_S) || (ofn_mode != ROCKER_K_MODE_S_C) || ((ofn_mode != ROCKER_K_MODE_L) || (ofn_mode != ROCKER_K_MODE_L_8)) || (ofn_mode != ROCKER_K_MODE_L_C))
				#else
					if(ofn_mode != JOYSTICK_MODE)
				#endif
					{
						keyboard_dir = INSERT;			//Assigns the keyboard INSERT key to left click 
					}
					else
					{
						buttons |= 0x40;	// Set Joystick Left Button
					}
					/// Modified by TLLAU END ///

				seq3_state = 20;				
			}
			break;
		case 20:
			if(LEFT_SW == 1)
			{
				t3_data = DEBOUNCE_TIME;
				time_3 = 1;
				seq3_state = 30;
			}
			break;
		case 30:
			if(LEFT_SW == 0)
			{
				seq3_state = 20;
			}
			else if(time_3 == 0)
			{

				#ifdef JOYSTICK
					if((ofn_mode != JOYSTICK_MODE)||(ofn_mode != ROCKER_K_MODE_S) || (ofn_mode != ROCKER_K_MODE_S_C) || ((ofn_mode != ROCKER_K_MODE_L) || (ofn_mode != ROCKER_K_MODE_L_8)) || (ofn_mode != ROCKER_K_MODE_L_C))
				#else
					if(ofn_mode != JOYSTICK_MODE)
				#endif
					{			
						buttons &= ~LEFT_BUTTON;
					}
					else
					{
						buttons &= ~0x40;	// Clear Joystick Left Button	
					}
					/// Modified by TLLAU END ///

				seq3_state = 0;
			}
			break;
		default:
			break;
	}
}


//------------------------------------------------------------------------------------
// Right_Switch (Right Switch)
//------------------------------------------------------------------------------------

void Right_Switch(void)
{
	switch(seq4_state)
	{
		case 0:
			if(RIGHT_SW == 0)
			{
				t4_data = DEBOUNCE_TIME;
				time_4 = 1;
				seq4_state = 10;
			}
			break;
		case 10:
			if(RIGHT_SW == 1)
			{
				seq4_state = 0;
			}
			else if(time_4 == 0)
			{
				free_scroll = 0;
				buffer_clear();
				count_z = 0;




				#ifdef JOYSTICK
					if((ofn_mode == JOYSTICK_MODE) ||(ofn_mode == ROCKER_K_MODE_S) || (ofn_mode == ROCKER_K_MODE_S_C) || ((ofn_mode == ROCKER_K_MODE_L) || (ofn_mode == ROCKER_K_MODE_L_8)) || (ofn_mode == ROCKER_K_MODE_L_C))
				#else
					if(ofn_mode == JOYSTICK_MODE)
				#endif
					{
						buttons |= 0x80;	// Set Joystick Right Button
					}
					/// Modified by TLLAU END ///
				t4_data = 2000;
				time_4 = 1;
				seq4_state = 20;
			}
			break;
		case 20:
			if(RIGHT_SW == 1)
			{


				#ifdef JOYSTICK
					if((ofn_mode != JOYSTICK_MODE) || (ofn_mode != ROCKER_K_MODE_S) || (ofn_mode != ROCKER_K_MODE_S_C) || ((ofn_mode != ROCKER_K_MODE_L) || (ofn_mode != ROCKER_K_MODE_L_8)) || (ofn_mode != ROCKER_K_MODE_L_C))
				#else
					if(ofn_mode != JOYSTICK_MODE)
				#endif
					{
						if(ofn_mode == SCROLL_MODE )
						{
							buttons &= ~RIGHT_BUTTON;
						}
						else
						{
		//					buttons |= RIGHT_BUTTON;
							keyboard_dir = BACKSPACE;			//Assigns the keyboard BACKSPACE key to right click 
						}
					}
					/// Modified by TLLAU END ///
				
				t4_data = DEBOUNCE_TIME;
				time_4 = 1;
				seq4_state = 30;
			}
			else if(time_4 == 0)
			{	// function
				if(communication_mode == SPI_MODE)
				{
					Initiliaze_A320_I2C();
					communication_mode = I2C_MODE;
//					L6 = 1; // Modified by TLLAU
					led_temp = ~P1;
					P1 &= ~0xFC;
					P1 |= (led_temp & 0xFC);
						
				/*	i2c_enable();
					communication_mode = I2C_MODE;
//					L6 = 1; // Modified by TLLAU
				*/
				}
				else if(communication_mode == I2C_MODE)
				{
					Initiliaze_A320();
					communication_mode = SPI_MODE;
//					L6 = 0; // Modified by TLLAU
					led_temp = ~P1;
					P1 &= ~0xFC;
					P1 |= (led_temp & 0xFC);

				/*	spi_enable();
					communication_mode = SPI_MODE;
//					L6 = 0; // Modified by TLLAU
				*/
				}
				seq4_state = 25;
			}
			break;
		case 25:
			if(RIGHT_SW == 1)
			{
				t4_data = DEBOUNCE_TIME;
				time_4 = 1;
				seq4_state = 30;
			}
			break;
		case 30:
			if(RIGHT_SW == 0)
			{
				seq4_state = 20;
			}
			else if(time_4 == 0)
			{

				#ifdef JOYSTICK
					if((ofn_mode != JOYSTICK_MODE) || (ofn_mode != ROCKER_K_MODE_S) || (ofn_mode != ROCKER_K_MODE_S_C) || ((ofn_mode != ROCKER_K_MODE_L) || (ofn_mode != ROCKER_K_MODE_L_8)) || (ofn_mode != ROCKER_K_MODE_L_C))
				#else
					if(ofn_mode != JOYSTICK_MODE)
				#endif
					{
						buttons &= ~RIGHT_BUTTON;
	//					buttons &= ~BACKSPACE;	//CWL
					}
					else
					{
						buttons &= ~0x80;	// Clear Joystick Right Button
					}
					/// Modified by TLLAU END ///
				
				seq4_state = 0;
			}
			break;
		default:
			break;
	}
}


//------------------------------------------------------------------------------------
// Mode_Select_Switch (Middle Switch)
//------------------------------------------------------------------------------------

void Mode_Select_Switch(void)
{
	switch(seq5_state)
	{
		case 0:
			if(MODE_SEL_SW == 0)
			{
				t5_data = DEBOUNCE_TIME;
				time_5 = 1;
				seq5_state = 10;
			}
			break;
		case 10:
			if(MODE_SEL_SW == 1)
			{
				seq5_state = 0;
			}
			else if(time_5 == 0)
			{
				// function
				rc_flag = 1;
				free_scroll = 0;
				count_z = 0;

				ofn_mode++;			// set OFN Mode (FINGER_MODE, Single Step Rocker, Multiple Step Rocker, Single Step Continuous Rocker and Multiple Step Continuous Rocker)
				if(ofn_mode > MODE_NUM)
				{
					ofn_mode = 0;
				}
				
				switch(ofn_mode)
				{
					case FINGER_MODE:
						// set to Navigation Mode
						L2 = 0;
						L3 = 1;
						L4 = 1;
						L5 = 1;
						L6 = 1; // Modified by TLLAU
						LED7 = 1; // Modified by TLLAU
						threshold_flag = 0;
						buffer_clear();
						count_z = 0;
						SHUTTER_LOWER_TRES = shutter_motion1;
						SHUTTER_UPPER_TRES = shutter_motion2;
						shutter_flag_temp = SHUTTER_DELAY;
						break;

					case SCROLL_MODE:
						// reset to Scroll Mode	
						L2 = 1; // 0; Modified by TLLAU		
						L3 = 1; // 0; Modified by TLLAU
						L4 = 1;
						L5 = 0; // 1; Modified by TLLAU	
						L6 = 1; // Modified by TLLAU
						LED7 = 1; // Modified by TLLAU
						buffer_clear();
						count_z = 0;
						SHUTTER_LOWER_TRES = shutter_motion1;
						SHUTTER_UPPER_TRES = shutter_motion2;
						shutter_flag_temp = SHUTTER_DELAY;
						break;						
/*
					case ROCKER_K_MODE_S:
						// set to Single Step Rocker (Keyboard) Mode	
						L2 = 0;
						L3 = 1;
						L4 = 1;
						L5 = 1;
						L6 = 1; // Modified by TLLAU
						LED7 = 1; // Modified by TLLAU
						threshold_flag = 1;
						keyboard_dir = 0;
						keyboard_dir_temp = 0;
						SHUTTER_LOWER_TRES = shutter_basic + ((shutter_d * 1) / 10);
						SHUTTER_UPPER_TRES = shutter_basic + ((shutter_d * 1) / 10);
						shutter_flag_temp = 0x0000;
						break;
*/
					case ROCKER_K_MODE_L:
						// set to Multiple Step Rocker (Keyboard) Mode	
						eight_way = 0; // Modified by TLLAU		
						L2 = 1;
						L3 = 1;
						L4 = 0;	// 1; Modified by TLLAU	
						L5 = 1;	// 0; Modified by TLLAU
						L6 = 1; // Modified by TLLAU
						LED7 = 1; // Modified by TLLAU		
						keyboard_dir = 0;
						keyboard_dir_temp = 0;
						SHUTTER_LOWER_TRES = shutter_basic + ((shutter_d * 1) / 10);
						SHUTTER_UPPER_TRES = shutter_basic + ((shutter_d * 1) / 10);
						shutter_flag_temp = 0x0000;
						break;
					case ROCKER_K_MODE_S_C:
						// set to Single Step Continuous Rocker (Keyboard) Mode	
						L2 = 1;
						L3 = 0;
						L4 = 1;
						L5 = 1;
						L6 = 1; // Modified by TLLAU
						LED7 = 1; // Modified by TLLAU
						keyboard_dir = 0;
						keyboard_dir_temp = 0;
						SHUTTER_LOWER_TRES = shutter_basic + ((shutter_d * 1) / 10);
						SHUTTER_UPPER_TRES = shutter_basic + ((shutter_d * 1) / 10);
						shutter_flag_temp = 0x0000;
						break;

					case ROCKER_K_MODE_L_C:
						// set to Multiple Step Continuous Rocker (Keyboard) Mode	
						L2 = 1;
						L3 = 1;
						L4 = 0;
						L5 = 1;
						L6 = 1; // Modified by TLLAU
						LED7 = 1; // Modified by TLLAU
						keyboard_dir = 0;
						keyboard_dir_temp = 0;
						SHUTTER_LOWER_TRES = shutter_basic + ((shutter_d * 1) / 10);
						SHUTTER_UPPER_TRES = shutter_basic + ((shutter_d * 1) / 10);
						shutter_flag_temp = 0x0000;
						break;

					/// Modified by TLLAU BEGIN ///
					case JOYSTICK_MODE:	// Add-in Joystick Mode
						// set to Joystick Mode (Eight Direction)	
						L2 = 1;	// 0; Modified by TLLAU			
						L3 = 1;	// 0; Modified by TLLAU		
						L4 = 1;	// 0; Modified by TLLAU		
						L5 = 1;	// 0; Modified by TLLAU		
						L6 = 0; // Modified by TLLAU
						LED7 = 1; // Modified by TLLAU
						keyboard_dir = 0;
						keyboard_dir_temp = 0;
						SHUTTER_LOWER_TRES = shutter_basic + ((shutter_d * 1) / 10);
						SHUTTER_UPPER_TRES = shutter_basic + ((shutter_d * 1) / 10);
						shutter_flag_temp = 0x0000;
						break;
					
					case ROCKER_K_MODE_L_8:
						// set to Multiple Step Rocker (Keyboard) Mode	
						eight_way = 1; // Modified by TLLAU		
						L2 = 1;
						L3 = 1;
						L4 = 1;	// 1; Modified by TLLAU	
						L5 = 1;	// 0; Modified by TLLAU
						L6 = 1; // Modified by TLLAU
						LED7 = 0; // Modified by TLLAU		
						keyboard_dir = 0;
						keyboard_dir_temp = 0;
						SHUTTER_LOWER_TRES = shutter_basic + ((shutter_d * 1) / 10);
						SHUTTER_UPPER_TRES = shutter_basic + ((shutter_d * 1) / 10);
						shutter_flag_temp = 0x0000;
						break;					
					/// Modified by TLLAU END ///
					default:
						break;
				}

				if(communication_mode == I2C_MODE)
				{
					led_temp = ~P1;
					P1 &= ~0xFC;
					P1 |= (led_temp & 0xFC);
				}

				seq5_state = 20;
			}
			break;
		case 20:
			if(MODE_SEL_SW == 1)
			{
				t5_data = DEBOUNCE_TIME;
				time_5 = 1;
				seq5_state = 30;
			}
			break;
		case 30:
			if(MODE_SEL_SW == 0)
			{
				seq5_state = 20;
			}
			else if(time_5 == 0)
			{
				buttons &= ~MIDDLE_BUTTON;
				seq5_state = 0;
			}
			break;
		default:
			break;
	}
}

//------------------------------------------------------------------------------------
// Dome_Switch (Right Switch)
//------------------------------------------------------------------------------------

void Dome_Switch(void)
{
	switch(seq7_state)
	{
		case 0:
			if(DOME == 0)
			{
				t7_data = DEBOUNCE_TIME;
				time_7 = 1;
				seq7_state = 10;
			}
			break;
		case 10:
			if(DOME == 1)
			{
				seq7_state = 0;
			}
			else if(time_7 == 0)
			{
				free_scroll = 0;
				buffer_clear();
				count_z = 0;
				//buttons |= LEFT_BUTTON;


				#ifdef JOYSTICK
					if((ofn_mode != JOYSTICK_MODE)||(ofn_mode != ROCKER_K_MODE_S) || (ofn_mode != ROCKER_K_MODE_S_C) || ((ofn_mode != ROCKER_K_MODE_L) || (ofn_mode != ROCKER_K_MODE_L_8)) || (ofn_mode != ROCKER_K_MODE_L_C))
				#else
					if(ofn_mode != JOYSTICK_MODE)
				#endif	
					{
						if(ofn_mode == FINGER_MODE)
						{
							buttons |= LEFT_BUTTON;	
						}
						else
						{
							keyboard_dir = ENTER_KEY;				//Assigns the keyboard ENTER key to dome switch
						}
					}
					else
					{
						buttons |= 0x40;	// Set Joystick Left Button
					}
					/// Modified by TLLAU END ///

				seq7_state = 20;
			}
			break;
		case 20:
			if(DOME == 1)
			{
				t7_data = DEBOUNCE_TIME;
				time_7 = 1;
				seq7_state = 30;
			}
			break;
		case 30:
			if(DOME == 0)
			{
				seq7_state = 20;
			}
			else if(time_7 == 0)
			{


				#ifdef JOYSTICK
					if((ofn_mode != JOYSTICK_MODE)||(ofn_mode != ROCKER_K_MODE_S) || (ofn_mode != ROCKER_K_MODE_S_C) || ((ofn_mode != ROCKER_K_MODE_L) || (ofn_mode != ROCKER_K_MODE_L_8)) || (ofn_mode != ROCKER_K_MODE_L_C))
				#else
					if(ofn_mode != JOYSTICK_MODE)
				#endif
					{
						//right_button = 0;
						buttons &= ~LEFT_BUTTON;
					}
					else
					{
						buttons &= ~0x40;	// Clear Joystick Left Button
					}
					/// Modified by TLLAU END ///

				seq7_state = 0;
			}
			break;
		default:
			break;
	}
}


//------------------------------------------------------------------------------------
// Motion_SPInTWI  (Motion - SPI & TWI)
//------------------------------------------------------------------------------------


void Motion_SPInTWI (void)
{
//	unsigned char bla;
	
	/// Modified by TLLAU BEGIN ///
	// Add-in Joystick Mode
	if(ofn_mode == JOYSTICK_MODE)
	{
		switch(seq6_state)
		{
			case 0:			
				if(time_6 == 0)
				{
					if(MOTION == 0)
					{
						if(communication_mode == SPI_MODE)
						{
						#ifdef MOTION_BURST
							spi_read_motionburst(motion_buffer);
							delta_y = motion_buffer[1];
							delta_x = motion_buffer[2];
							shutter6.b[0] = motion_buffer[4];
							shutter6.b[1] = motion_buffer[5];
							pix_max = motion_buffer[6];
							pix_min = motion_buffer[8];

							spi_temp = delta_y;
							delta_y = delta_x;
							delta_x = spi_temp;
						#else
							delta_x = spi_read_sensor(ADBM_A320_DELTAX_ADDR);
							delta_y = spi_read_sensor(ADBM_A320_DELTAY_ADDR);
							pix_max = spi_read_sensor(ADBM_A320_MAXIMUMPIXEL_ADDR);
							pix_min = spi_read_sensor(ADBM_A320_MINIMUMPIXEL_ADDR);
						#endif
						}
						else if(communication_mode == I2C_MODE)
						{
							while(i2c_status != I2C_FREE);
							i2c_ByteRead(I2C_ADDR, ADBM_A320_MOTION_ADDR, &motion_buffer[0], 9); 
							while(i2c_status != I2C_FREE);
							delta_x = motion_buffer[1];
							delta_y = motion_buffer[2];
							shutter6.b[0] = motion_buffer[4];
							shutter6.b[1] = motion_buffer[5];
							pix_max = motion_buffer[6];
							pix_min = motion_buffer[8];
						}
					
						count_x_temp = delta_x;
						count_y_temp = delta_y;

						data_process();
					}
					else
					{
						buffer_clear();
					}
					
					speed_switching();

					if((ofn_mode == FINGER_MODE) || (ofn_mode == SCROLL_MODE))
					{
						t6_data = 0;//SENSOR_TIME;
					}
					else if(keyboard_tap != 0)
					{
						L7 = 0;
						X_Total_Displacement = 0;
						Y_Total_Displacement = 0;
						keyboard_tap_repeat = keyboard_tap;
						keyboard_tap = 0;
						t6_data = KEYPRESS_DELAY;
						time_6 = 1;
					}
					
					seq6_state = 10;
				}
				break;
			case 10:
				if(time_6 == 0)
				{					
					if(communication_mode == SPI_MODE)
					{
						shutter1.b[0] = spi_read_sensor(ADBM_A320_SHUTTERUPPER_ADDR);
						shutter1.b[1] = spi_read_sensor(ADBM_A320_SHUTTERLOWER_ADDR);
					}
					else if(communication_mode == I2C_MODE)
					{
						while(i2c_status != I2C_FREE);
						i2c_ByteRead(I2C_ADDR, ADBM_A320_SHUTTERUPPER_ADDR, &motion_buffer[4], 2); 
						while(i2c_status != I2C_FREE);
						shutter1.b[0] = motion_buffer[4];
						shutter1.b[1] = motion_buffer[5];
					}
					
					//shutter1.b[0] = spi_read_sensor(ADBM_A320_SHUTTERUPPER_ADDR);
					//shutter1.b[1] = spi_read_sensor(ADBM_A320_SHUTTERLOWER_ADDR);
				
					//if(shutter1.w < SHUTTER_THERS_LO)
					if(GPIO)
					{
						L7 = 0;
						X_Total_Displacement = 0;
						Y_Total_Displacement = 0;
						keyboard_dir = keyboard_tap_repeat;
						keyboard_tap1 = keyboard_tap_repeat;
						if((ofn_mode == ROCKER_M_MODE) && (ofn_mode == ROCKER_K_MODE))
						{
							t6_data = KEYPRESS_DELAY;
							time_6 = 1;
						}
						else
						{
							t6_data = SENSOR_TIME;
							time_6 = 1;
							seq6_state = 0;
						}
					}

					//if(shutter1.w > SHUTTER_THERS_UP)
					else
					{
						L7 = 1;
						X_Total_Displacement = 0;
						Y_Total_Displacement = 0;
						keyboard_dir = 0;
						keyboard_tap1 = 0;
						keyboard_tap_repeat = 0;
						delta_x = 0;
						delta_y = 0;
						time_6 = 0;
						seq6_state = 0;							
					}	
				}
				break;
			default:
				break;
		}
	}
	/// Modified by TLLAU END ///
	else
	{
		seq6_state = 0;

		switch(seq6_state)
		{
			case 0:			
				//time_6 is a timer that triggers every 8ms
				if(time_6 == 0)
				{
			
				// This function is to check and negate FPD whenever finger is not present or when 
				// there is no motion
				// This overwriting loop can be performed at 20ms interval with no noticeable effect to FPD overwrite effectiveness
					if(time_14 == 0)
					{
						/// NOT PART OF FPD CODE BEGIN ///
						if(communication_mode == SPI_MODE)
						{
							shutter1.b[0] = spi_read_sensor(ADBM_A320_SHUTTERUPPER_ADDR);
							shutter1.b[1] = spi_read_sensor(ADBM_A320_SHUTTERLOWER_ADDR);
						}
						else if(communication_mode == I2C_MODE)
						{
							while(i2c_status != I2C_FREE);
							i2c_ByteRead(I2C_ADDR, ADBM_A320_SHUTTERUPPER_ADDR, &motion_buffer[4], 2); 
							while(i2c_status != I2C_FREE);
							shutter1.b[0] = motion_buffer[4];
							shutter1.b[1] = motion_buffer[5];
						}

						//shutter1.b[0] = spi_read_sensor(ADBM_A320_SHUTTERUPPER_ADDR);
						//shutter1.b[1] = spi_read_sensor(ADBM_A320_SHUTTERLOWER_ADDR);

						shutter_flag = shutter_flag << 1;
						if (L7 == 0)
						{
							shutter_flag = shutter_flag | 0x0001;
						}

						if (L7 == 1)
						{
						}
						/// NOT PART OF FPD CODE END ///


						if ((motion2 & 0x01))
						{	// For illustration these registers are read in normal mode.
							// Due to the number of registers to read, it's recommended to use polling mode 
							// to save reading time.
							
								if(communication_mode == SPI_MODE)
								{
									motion  = spi_read_sensor(ADBM_A320_MOTION_ADDR);
									squal   = spi_read_sensor(ADBM_A320_SQUAL_ADDR);
									pix_avg = spi_read_sensor(ADBM_A320_PIXELSUM_ADDR);
									pix_min = spi_read_sensor(ADBM_A320_MINIMUMPIXEL_ADDR);
									pix_max = spi_read_sensor(ADBM_A320_MAXIMUMPIXEL_ADDR);
									shutter1.b[0] = spi_read_sensor(ADBM_A320_SHUTTERUPPER_ADDR);
									shutter1.b[1] = spi_read_sensor(ADBM_A320_SHUTTERLOWER_ADDR);

								}
								else if(communication_mode == I2C_MODE)
								{
									while(i2c_status != I2C_FREE);
									i2c_ByteRead(I2C_ADDR, ADBM_A320_MOTION_ADDR, &motion_buffer[0], 9); 
									while(i2c_status != I2C_FREE);
									motion  = motion_buffer[0];
									squal   = motion_buffer[3];
									shutter1.b[0] = motion_buffer[4];
									shutter1.b[1] = motion_buffer[5];
									pix_max = motion_buffer[6];
									pix_avg = motion_buffer[7];
									pix_min = motion_buffer[8];
								}
					
																							
					/*		motion  = spi_read_sensor(ADBM_A320_MOTION_ADDR);
							squal   = spi_read_sensor(ADBM_A320_SQUAL_ADDR);
							pix_avg = spi_read_sensor(ADBM_A320_PIXELSUM_ADDR);
							pix_min = spi_read_sensor(ADBM_A320_MINIMUMPIXEL_ADDR);
							pix_max = spi_read_sensor(ADBM_A320_MAXIMUMPIXEL_ADDR);
							shutter1.b[0] = spi_read_sensor(ADBM_A320_SHUTTERUPPER_ADDR);
							shutter1.b[1] = spi_read_sensor(ADBM_A320_SHUTTERLOWER_ADDR);
					*/

							//Method 2
							///////////////////////////////////////////////////////
							///////////////////////////////////////////////////////
							motion2 = motion;

							// If pix_min and pix_min difference is more than 240, it tends to mean that the 
							// light source is strong.
							// Whereas squal value is used as a check when unit is looking at uniform scene, 
							// which tends to signify finger off case.
							// Shutter threshold is check too. Very low shutter tends to mean light is shining 
							// directly into sensor.
							if ( (abs(pix_max - pix_min) > 240) || (squal < 15) || (shutter1.w < 25) )
							{	
								if (finger_leave_counter2 >= 8)
								{
									motion2 = motion2 & 0xFE;
									finger_leave_counter2 = 0;
									finger_on_counter = 0;
		//							free_scroll = 0;
								}
								else
								{
									finger_leave_counter2++;
								}

							}
 	 
							if ((motion2 & 0x01) == 0)
							{
								L7 = !(motion2 & 0x01);			// show FPD status on LED
								L1 = L7;	//CWL 090109
							}

							///////////////////////////////////////////////////////
							///////////////////////////////////////////////////////

							t14_data = 20;
							time_14 = 1;

						}

					} //20 milisecond overwrite speed.


					// If MOTION pin is low (or active) the follow sequence is performed.
					// The function included in here are:
					// 1) read delta_x and delta_y for reporting
					// 2) read other pixel statistics for FPD negation
					if(MOTION == 0)
					{
						if(communication_mode == SPI_MODE)
						{
						#ifdef MOTION_BURST
							spi_read_motionburst(motion_buffer);
							delta_y = motion_buffer[1];
							delta_x = motion_buffer[2];
							shutter6.b[0] = motion_buffer[4];
							shutter6.b[1] = motion_buffer[5];
							pix_max = motion_buffer[6];
							pix_min = motion_buffer[8];

							spi_temp = delta_y;
							delta_y = delta_x;
							delta_x = spi_temp;
						#else
							delta_x = spi_read_sensor(ADBM_A320_DELTAX_ADDR);
							delta_y = spi_read_sensor(ADBM_A320_DELTAY_ADDR);
							pix_max = spi_read_sensor(ADBM_A320_MAXIMUMPIXEL_ADDR);
							pix_min = spi_read_sensor(ADBM_A320_MINIMUMPIXEL_ADDR);
						#endif
						}
						else if(communication_mode == I2C_MODE)
						{
							while(i2c_status != I2C_FREE);
							i2c_ByteRead(I2C_ADDR, ADBM_A320_MOTION_ADDR, &motion_buffer, 7); 
							while(i2c_status != I2C_FREE);
							delta_x = motion_buffer[1];
							delta_y = motion_buffer[2];
							shutter6.b[0] = motion_buffer[4];
							shutter6.b[1] = motion_buffer[5];
							pix_max = motion_buffer[6];
							pix_min = motion_buffer[8];
						}	
		/*			
						#ifdef MOTION_BURST
							spi_read_motionburst(&motion, &delta_x, &delta_y, &squal, &shutter6.b[0], &shutter6.b[1]);
						#else
							motion  = spi_read_sensor(ADBM_A320_MOTION_ADDR);
							delta_x = spi_read_sensor(ADBM_A320_DELTAX_ADDR);
							delta_y = spi_read_sensor(ADBM_A320_DELTAY_ADDR);
							squal   = spi_read_sensor(ADBM_A320_SQUAL_ADDR);
							shutter6.b[0] = spi_read_sensor(ADBM_A320_SHUTTERUPPER_ADDR);
							shutter6.b[1] = spi_read_sensor(ADBM_A320_SHUTTERLOWER_ADDR);
						#endif

						shutter1.b[0] = spi_read_sensor(ADBM_A320_SHUTTERUPPER_ADDR);
						shutter1.b[1] = spi_read_sensor(ADBM_A320_SHUTTERLOWER_ADDR);
						pix_avg = spi_read_sensor(ADBM_A320_PIXELSUM_ADDR);
						pix_min = spi_read_sensor(ADBM_A320_MINIMUMPIXEL_ADDR);
						pix_max = spi_read_sensor(ADBM_A320_MAXIMUMPIXEL_ADDR);
*/

	//CWL
						if(TrackSW == 0)
						{
							delta_x = delta_y = 0;
						}


						count_x_temp = delta_x;
						count_y_temp = delta_y;


						///////////////////////////////////////////////////////
						//Method 2
						///////////////////////////////////////////////////////

						if ( (delta_x != 0) || (delta_y != 0)) 
						{ 	finger_leave_counter2 = 0;
							finger_on_counter++;
							if (finger_on_counter > 150)
							{ finger_on_counter = 149;}

						  	motion2 = motion2 | 0x01;
						}
					
						// Same comparison is done here as above, except that it now includes delta x y into the equation
						if ( ( (abs(pix_max - pix_min) > 240) || (squal < 15) || (shutter1.w < 25)) && (delta_x == 0) && (delta_y == 0))
						{	delta_x = 0;
							delta_y = 0;
							// Hysteresis of a different time is applied here.
							if (finger_leave_counter2 > 5)
							{	motion2 = motion2 & 0xFE;
								finger_leave_counter2 = 0;
								finger_on_counter = 0;
							}
							else
							{	motion2 = motion2 | 0x01;}

						}

 
	 					// A delay of 5 cycles is added here to reduce false FPD deactivation during tracking.
						if (finger_on_counter >= 5)
						{
							motion2 = motion2 | 0x01;
						}
						else
						{   motion2 = motion2 & 0xFE;
						}


						L7 = !(motion2 & 0x01);		// show FPD status on LED
						L1 = L7;	//CWL 090109
						///////////////////////////////////////////////////////
						///////////////////////////////////////////////////////

		
						data_process();

						send_rocker_data = 0;

						t_continuous_rock_trigger = 1;
						t_continuous_rock_trigger_data = 300;

						if ((ofn_mode == ROCKER_K_MODE_L) || (ofn_mode == ROCKER_K_MODE_L_8))
						{
							time_16 = 1;
							t16_data = 800;
						}



					}
					else
					{
						buffer_clear();
					}

					speed_switching();


					t6_data = SENSOR_TIME;
					time_6 = 1;

					finger_status = L7;



					if ((time_16 == 0)&&(finger_status == 0)&&((ofn_mode == ROCKER_K_MODE_L) || (ofn_mode == ROCKER_K_MODE_L_8)))
					{
						if (start_continuous_stepping == 0)
						{
							continuous_delta_x = rocker_delta_x;
							continuous_delta_y = rocker_delta_y;
						}

						data_process();		
	//CWL							
						if ((ofn_mode == ROCKER_K_MODE_S ) || (((ofn_mode == ROCKER_K_MODE_L ) || (ofn_mode == ROCKER_K_MODE_L_8)) && (time_16 != 0)))
						{
							start_continuous_stepping = 0;
						}
						else
						{
							start_continuous_stepping = 1;
						}
					}
					 

					if ((t_continuous_rock_trigger == 0) && (finger_status == 0) && ((ofn_mode != ROCKER_K_MODE_L)|| (ofn_mode != ROCKER_K_MODE_L_8)))
					{	
						if ((start_continuous_stepping == 0) /*|| (time_16 != 0)*/)
						{	
							continuous_delta_x = rocker_delta_x;
							continuous_delta_y = rocker_delta_y;
						}
						
					


						data_process();		
	//CWL							
						if ((ofn_mode == ROCKER_K_MODE_S ) || (((ofn_mode == ROCKER_K_MODE_L ) || (ofn_mode == ROCKER_K_MODE_L_8)) && (time_16 != 0)))
						{
							start_continuous_stepping = 0;
						}
						else
						{
							start_continuous_stepping = 1;
						}

					}

	//Acceleration parameter settings in Single Step Continuous Rocker Mode
					if ( (start_continuous_stepping == 1) && (t_continuous_rock_interval == 0) && (ofn_mode == ROCKER_K_MODE_S_C) )// && (send_rocker_data == 1) )
					{	t_continuous_rock_interval = 1;
						if (issued_times < n1)						// 2	
							t_continuous_rock_interval_data = t1;	// 500
						else if (issued_times < n2)					// 10
							t_continuous_rock_interval_data = t2;  // 250
						else if (issued_times < n3)					// 26  
							t_continuous_rock_interval_data = t3;  // 125
						else if (issued_times < n4)					// 58 
							t_continuous_rock_interval_data = t4;  	// 62
						else if (issued_times > n4)					// 58 
							t_continuous_rock_interval_data = t5;   // 31


	//CWL					t_continuous_rock_interval_data = 250;

						if (start_continuous_stepping == 1) 
						{	rocker_delta_x = continuous_delta_x;
							rocker_delta_y = continuous_delta_y;
							issued_times++;
							if (issued_times > 250)
								issued_times = 251;
						}

						data_process();
					//	send_rocker_data = 0;
					}



	//Acceleration paremeter settings in Multiple Step Continuous Rocker Mode
					if ( (start_continuous_stepping == 1) && (t_continuous_rock_interval == 0) && (ofn_mode == ROCKER_K_MODE_L_C) || ( ((ofn_mode == ROCKER_K_MODE_L) || (ofn_mode == ROCKER_K_MODE_L_8)) && (time_16 == 0) && (t_continuous_rock_interval == 0) ))// && (send_rocker_data == 1) )
					{	t_continuous_rock_interval = 1;
						if (issued_times < n1)						// 2	
							t_continuous_rock_interval_data = t1;	// 500
						else if (issued_times < n2)					// 10
							t_continuous_rock_interval_data = t2;  // 250
						else if (issued_times < n3)					// 26  
							t_continuous_rock_interval_data = t3;  // 125
						else if (issued_times < n4)					// 58 
							t_continuous_rock_interval_data = t4;  	// 62
						else if (issued_times > n4)					// 58
							t_continuous_rock_interval_data = t5;   // 31

						if ((start_continuous_stepping == 1) /*|| ( ((ofn_mode == ROCKER_K_MODE_L) || (ofn_mode == ROCKER_K_MODE_L_8)) && (time_16 == 0) )*/)
						{
							rocker_delta_x = continuous_delta_x;
							rocker_delta_y = continuous_delta_y;
							issued_times++;
							multiple_step_rocker_flag = 1;
							if (issued_times > 250)
								issued_times = 251;
						}
						data_process();
					//	send_rocker_data = 0;
					}




					if ( ( (finger_status == 1) && (send_rocker_data == 0) ) || ( (start_continuous_stepping == 1) && (t_continuous_rock_interval == 0) ) || ( ((ofn_mode == ROCKER_K_MODE_L) || (ofn_mode == ROCKER_K_MODE_L_8)) && (time_16 == 0) && (t_continuous_rock_interval == 0) ))
					{
						issued_times++;
						if ((start_continuous_stepping == 1) /*|| ( ((ofn_mode == ROCKER_K_MODE_L) || (ofn_mode == ROCKER_K_MODE_L_8)) && (time_16 == 0) )*/)
						{	rocker_delta_x = continuous_delta_x;
							rocker_delta_y = continuous_delta_y;

						}
						data_process();
					}

					if (finger_status == 1)
					{
						start_continuous_stepping = 0;
						issued_times = 0;		//CWL
//						continuous_delta_x = continuous_delta_y = 0;
					}


				}
				break;
			default:
				break;
		}
	}
}





//------------------------------------------------------------------------------------
// USB_Send_Data (Conditions the data to the respective USB format prior to sending the data out)
//------------------------------------------------------------------------------------

void USB_Send_Data(void)
{
	switch(seq11_state)
	{
		case 0:
			if(usb_mode == 1)
			{
				seq11_state = 10;
			}
			break;
		case 10:
			/// Modified by TLLAU BEGIN ///
			// Joystick Data Packet (step data)
			// x=0,y=0			x=127,y=0		x=255,y=0
			// (UP-LEFT)		(UP)			(UP-RIGHT)

			// x=0,y=127		x=127,y=127		x=255,y=127
			// (LEFT)			(CENTRE)		(RIGHT)

			// x=0,y=255		x=127,y=255		x=255,y=255
			// (DOWN-LEFT)		(DOWN)			(DOWN-RIGHT)
           


		#ifdef JOYSTICK
			if((ofn_mode == ROCKER_K_MODE_S) || (ofn_mode == ROCKER_K_MODE_S_C) || ((ofn_mode == ROCKER_K_MODE_L) || (ofn_mode == ROCKER_K_MODE_L_8)) || (ofn_mode == ROCKER_K_MODE_L_C))
			{
				if (key_step == 0x52)	//UP
					if (key_step2 == 0)
						keyboard_dir = UP_KEY;
					else if (key_step2 == LEFT_KEY)
						keyboard_dir = UP_L_KEY;
					else if (key_step2 == RIGHT_KEY)
						keyboard_dir = UP_R_KEY;
					else
						keyboard_dir = 0;

				if (key_step == 0x51)	//DOWN
					if (key_step2 == 0)
						keyboard_dir = DOWN_KEY;
					else if (key_step2 == LEFT_KEY)
						keyboard_dir = DOWN_L_KEY;
					else if (key_step2 == RIGHT_KEY)
						keyboard_dir = DOWN_R_KEY;
					else
						keyboard_dir = 0;

				if (key_step == 0x4F)	//RIGHT
					if (key_step2 == 0)
						keyboard_dir = RIGHT_KEY;
					else if (key_step2 == UP_KEY)
						keyboard_dir = UP_R_KEY;
					else if (key_step2 == DOWN_KEY)
						keyboard_dir = DOWN_R_KEY;
					else
						keyboard_dir = 0;

				if (key_step == 0x50)	//LEFT
					if (key_step2 == 0)
						keyboard_dir = LEFT_KEY;
					else if (key_step2 == UP_KEY)
						keyboard_dir = UP_L_KEY;
					else if (key_step2 == DOWN_KEY)
						keyboard_dir = DOWN_L_KEY;
					else
						keyboard_dir = 0;
			}

		#endif

		#ifdef JOYSTICK
			if((ofn_mode == JOYSTICK_MODE) || (ofn_mode == ROCKER_K_MODE_S) || (ofn_mode == ROCKER_K_MODE_S_C) || ((ofn_mode == ROCKER_K_MODE_L) || (ofn_mode == ROCKER_K_MODE_L_8)) || (ofn_mode == ROCKER_K_MODE_L_C))
		#else
			if(ofn_mode == JOYSTICK_MODE) 
		#endif	
			{				
				switch(keyboard_dir)
				{
					case UP_KEY:
						count_x = 127;
						count_y = 0;
						break;
					case DOWN_KEY:
						count_x = 127;
						count_y = 255;
						break;
					case RIGHT_KEY:
						count_x = 255;
						count_y = 127;
						break;
					case LEFT_KEY:
						count_x = 0;
						count_y = 127;
						break;
					case UP_R_KEY:
						count_x = 255;
						count_y = 0;
						break;
					case DOWN_R_KEY:
						count_x = 255;
						count_y = 255;
						break;
					case UP_L_KEY:
						count_x = 0;
						count_y = 0;
						break;
					case DOWN_L_KEY:
						count_x = 0;
						count_y = 255;
						break;
					default:
						count_x = 127;
						count_y = 127;
						break;
				}

				IN_PACKET[0] = JOYSTICK_USB_MODE;
				IN_PACKET[1] = count_x;
				IN_PACKET[2] = count_y;	
				IN_PACKET[3] = buttons;
				IN_BUFFER.Ptr = IN_PACKET;
				IN_BUFFER.Length = JOYSTICK_DATA_LENGTH;

				x_raw = count_x;
				y_raw = count_y;
				k_raw = 0;

				t11_data = JOYSTICK_DELAY;
				seq11_state = 15;
			}		
			/// Modified by TLLAU END ///

			else if((ofn_mode == FINGER_MODE) || (ofn_mode == SCROLL_MODE) || (sent_buttons != buttons))
			{
				// Mouse Data Packet
				if(sent_buttons != buttons || count_x || count_y || count_z || sent_buttons != keyboard_dir)
				{
					sent_buttons = buttons;

					IN_PACKET[0] = MOUSE_MODE;
					IN_PACKET[1] = buttons;
					IN_PACKET[2] = count_x;	
					IN_PACKET[3] = -count_y;
					IN_PACKET[4] = count_z;
					IN_BUFFER.Ptr = IN_PACKET;
					IN_BUFFER.Length = MOUSE_DATA_LENGTH;

					x_raw = count_x;
					y_raw = count_y;
					k_raw = 0;
								
					count_x = 0;
					count_y = 0;
					count_x0 = 0;
					count_y0 = 0;

					if(free_scroll == 1)
	                {
	                    t11_data = 80;
					}
					else if(free_scroll == 0)
					{
						count_z = 0;
						t11_data = 2;
					}

					if(click_flag_temp != click_flag)
					{
						click_flag_temp = click_flag;
						if(click_flag != 0)
						{
							keyboard_dir = keyboard_tap1;
							click_flag--;
							click_flag = 0;
							if(click_flag != 0)
							{
								click_flag = 1;
							}
						}
					}

				seq11_state = 15;
				}
			}

			// Keyboard Data Packet (step data)
			#ifndef JOYSTICK
			else if((ofn_mode == ROCKER_K_MODE_S) || (ofn_mode == ROCKER_K_MODE_S_C) || ((ofn_mode == ROCKER_K_MODE_L) || (ofn_mode == ROCKER_K_MODE_L_8)) || (ofn_mode == ROCKER_K_MODE_L_C))
			{
				if(keyboard_dir != keyboard_dir_temp)
				{
					keyboard_dir_temp = keyboard_dir;
					click_flag = 0;
					t11_data = STEP_DELAY;
				}
				else if(click_flag != 0)
				{
					keyboard_dir = keyboard_tap1;
					click_flag = 0;
					t11_data = 1;
				}

				if(keyboard_dir != 0)
				{
					IN_PACKET[0] = KEY_MODE;
					IN_PACKET[1] = 0;
					IN_PACKET[2] = 0;
					IN_PACKET[3] = keyboard_dir;
					IN_PACKET[4] = key_step2;
					IN_PACKET[5] = 0;
					IN_PACKET[6] = 0;
					IN_PACKET[7] = 0;
					IN_PACKET[8] = 0;
					IN_BUFFER.Ptr = IN_PACKET;
					IN_BUFFER.Length = KEY_DATA_LENGTH;

					x_raw = 0;
					y_raw = 0;
					k_raw = keyboard_dir;

					keyboard_dir = 0;

					seq11_state = 15;
				}
			}
						
			#endif
			break;



		case 15:
			if(EP_STATUS[1] == EP_IDLE)
			{
				SendPacket();
				
				/// Modified by TLLAU BEGIN ///
				// Add-in Joystick Mode
				if((ofn_mode == ROCKER_K_MODE_S) || (ofn_mode == ROCKER_K_MODE_S_C) || ((ofn_mode == ROCKER_K_MODE_L) || (ofn_mode == ROCKER_K_MODE_L_8)) || (ofn_mode == ROCKER_K_MODE_L_C)
					|| (ofn_mode == JOYSTICK_MODE))
				/// Modified by TLLAU END ///
				{
						seq11_state = 17;
				}
				else
				{
					
					seq11_state = 16;
				}
				
			}
			break;

//CWL
		case 16:
			if(EP_STATUS[1] == EP_IDLE)
			{
				IN_PACKET[0] = KEY_MODE;
				IN_PACKET[1] = 0;
				IN_PACKET[2] = 0;
				IN_PACKET[3] = keyboard_dir;
				IN_PACKET[4] = 0;
				IN_PACKET[5] = 0;
				IN_PACKET[6] = 0;
				IN_PACKET[7] = 0;
				IN_PACKET[8] = 0;
				IN_BUFFER.Ptr = IN_PACKET;
					IN_BUFFER.Length = KEY_DATA_LENGTH;
				SendPacket();
					x_raw = 0;
					y_raw = 0;
					k_raw = keyboard_dir;

					keyboard_dir = 0;
				time_11 = 1;
				seq11_state = 17;	//cwl

			}
			break;


		case 17:
			if(EP_STATUS[1] == EP_IDLE)
			{
				IN_PACKET[0] = KEY_MODE;
				IN_PACKET[1] = 0;
				IN_PACKET[2] = 0;
				IN_PACKET[3] = 0;
				IN_PACKET[4] = 0;
				IN_PACKET[5] = 0;
				IN_PACKET[6] = 0;
				IN_PACKET[7] = 0;
				IN_PACKET[8] = 0;
				IN_BUFFER.Ptr = IN_PACKET;
				IN_BUFFER.Length = KEY_DATA_LENGTH;
				SendPacket();

				x_raw = 0;
				y_raw = 0;
				keyboard_dir = 0;
				//k_raw = 0;

				click_flag = 0;
				time_11 = 1;
				seq11_state = 20;
			}
			break;

		case 20:
			if(time_11 == 0)
			{
				if((num_tap > 0)  && ((ofn_mode == ROCKER_K_MODE_S) || (ofn_mode == ROCKER_K_MODE_S_C) || ((ofn_mode == ROCKER_K_MODE_L) || (ofn_mode == ROCKER_K_MODE_L_8)) || (ofn_mode == ROCKER_K_MODE_L_C)))
				{
					num_tap--;
					keyboard_dir = keyboard_dir_temp;
					t11_data = num_tap_count * 20;				//STEP_DELAY;
					time_11 = 1;
					num_tap_count++;
					if(t11_data > 200)
					{
						t11_data = 200;
					}
					seq11_state = 30;

					if (num_tap_count >= 7)
						if(((abs((signed char)delta_x) > 2 )|| (abs((signed char)delta_y)) > 2) || (L7 == 0))
						{
							num_tap = 0;
							delta_x = delta_y = X_Acceleration_Buffer = Y_Acceleration_Buffer = count_x0 = count_y0 = X_Total_Displacement = Y_Total_Displacement = continuous_delta_x = continuous_delta_y = 0;
							seq11_state = 0;
							keyboard_dir = 0x00;
//							delta_x = delta_y = X_Acceleration_Buffer = Y_Acceleration_Buffer = count_x0 = count_y0 = keyboard_dir = keyboard_tap = keyboard_tap1 = key_step2 = keyboard_tap_repeat = X_Total_Displacement = Y_Total_Displacement = continuous_delta_x = continuous_delta_y = 0;
							buffer_clear();	
							count_x0 = 0;
							count_y0 = 0;
							count_x1 = 0;
							count_y1 = 0;
						}
					
				}
				else
				{
					seq11_state = 0;
//					#ifndef JOYSTICK
					if (ofn_mode != JOYSTICK_MODE)
					{
//cwl					keyboard_dir = 0x00;
//					delta_x = delta_y = X_Acceleration_Buffer = Y_Acceleration_Buffer = count_x0 = count_y0 = keyboard_dir = key_step2 = keyboard_tap_repeat = X_Total_Displacement = Y_Total_Displacement = continuous_delta_x = continuous_delta_y = 0;
					delta_x = delta_y = X_Acceleration_Buffer = Y_Acceleration_Buffer = count_x0 = count_y0 = X_Total_Displacement = Y_Total_Displacement =  0;
					buffer_clear();
					count_x0 = 0;
					count_y0 = 0;
					count_x1 = 0;
					count_y1 = 0;
					}
//					#endif
				}


				if(threshold_flag == 0)
				{
					keyboard_dir = 0x00;
					seq11_state = 0;	
				}
			}
			break;
		case 30:
			if(time_11 == 0)
			{
				seq11_state = 0;
				
			}
			break;
		default:
			break;
	}
}


//------------------------------------------------------------------------------------
// Motion_TWI (Motion - TWI)
//------------------------------------------------------------------------------------

void Motion_TWI(void)
{
	switch(seq13_state)
	{
		case 0:
			if(i2c_status == I2C_FREE)
			{
				i2c_ByteRead(I2C_ADDR, ADBM_A320_SHUTTERUPPER_ADDR, &motion_buffer, 2); 
				seq13_state = 10;
			}
			break;
		case 10:
			if(i2c_status == I2C_FREE)
			{
				shutter1.b[0] = motion_buffer[0];
				shutter1.b[1] = motion_buffer[1];

				shutter_flag = shutter_flag << 1;
				if(shutter1.w < SHUTTER_UPPER_TRES)
				{
					shutter_flag = shutter_flag | 0x0001;
					L7 = 0;
				}

				if(shutter1.w > SHUTTER_LOWER_TRES)
				{
					L7 = 1;
					keyboard_dir = 0;	//keyboard_dir = 0 when finger lifted up  							
				}

				seq13_state = 20;
			}
			break;

		case 20:
			if(MOTION == 0)
			{
				seq13_state = 30;
			}
			else
			{
				seq13_state = 50;
			}
			break;
		case 30:
			if(i2c_status == I2C_FREE)
			{
				i2c_ByteRead(I2C_ADDR, ADBM_A320_MOTION_ADDR, &motion_buffer, 3); 
				seq13_state = 40;
			}
			break;
		case 40:
			if(i2c_status == I2C_FREE)
			{
				delta_x = motion_buffer[1];
				delta_y = motion_buffer[2];

				data_process();
				speed_switching();

				if((abs(count_x0) + abs(count_y0)) > 10)
				{
					left_state = 0;
					right_state = 0;
				}

				t6_data = SENSOR_TIME;
				time_6 = 1;
				seq13_state = 60;	
			}
			break;
		case 50:
			speed_switching();

			if((abs(count_x0) + abs(count_y0)) > 10)
			{
				left_state = 0;
				right_state = 0;
			}

			t6_data = SENSOR_TIME;
			time_6 = 1;
			seq13_state = 60;
			break;
		case 60:
			if(time_13 == 0)
			{
				seq13_state = 0;
			}
			break;
		default:
			break;
	}
}


void data_process(void)
{
	if(((shutter_flag & shutter_flag_temp) == shutter_flag_temp) || (threshold_flag == 0)) // change
	{
		// process deltaX and deltaY depend on OFN Mode
		switch(ofn_mode)
		{
			case FINGER_MODE:
				
				count_x0 += (signed char) delta_x * step_multiplier_mode1;
				count_y0 += (signed char) delta_y * step_multiplier_mode1;
				count_z = 0;
				break;

			case SCROLL_MODE:
				//if(((shutter_flag & shutter_flag_temp) == shutter_flag_temp) || (threshold_flag == 0))
				{
					if(((pix_max - pix_min) > 200) || (shutter6.w < 20))
					{
						buffer_clear();
						count_z = 0;
					}
					else
					{
						if(abs(delta_y) > abs(delta_x))
						{
							buffer_clear();
							mod_z =  mod_z + (signed char) delta_y;
								count_z += mod_z /8;//8;

								if(abs(3*mod_z) < (72))
							{
								free_scroll = 0;
							}
							else
							{
								free_scroll = 1;
								if(count_z < 0)
								{
									count_z = -1;
								}
								else
								{
									count_z = 1;
								}
							}
							mod_z = mod_z %8;//8;
						}
					}
				}
				break;


			case ROCKER_K_MODE_S:

				eight_way = 0;
				if ( (L7 == 0) && (start_continuous_stepping == 0) )
				{	
					rocker_delta_x=rocker_delta_x+(signed char)delta_x;
					rocker_delta_y=rocker_delta_y+(signed char)delta_y;
				}

				if ( (L7 == 1) || (start_continuous_stepping == 1) )
				{
					keyboard_tap = toggle_step(rocker_delta_x, rocker_delta_y);	// detect swipe
					if(keyboard_tap != 0)
					{
						keyboard_dir = keyboard_tap;
						keyboard_tap1 = keyboard_tap;							
					}
					buffer_clear();
					count_z = 0;
					rocker_delta_x = 0;
					rocker_delta_y = 0;
					send_rocker_data = 1;
				}

				buffer_clear();
				count_z = 0;
				break;

			case ROCKER_K_MODE_L:
			case ROCKER_K_MODE_L_8:
		
				if ((L7 == 0)  && (start_continuous_stepping == 0) /*&& (time_16 != 0)*/)
				{

					rocker_delta_x=rocker_delta_x+(signed char)delta_x;
					rocker_delta_y=rocker_delta_y+(signed char)delta_y;

					if ((delta_x >= 2) && (delta_y >= 2)) 
					{
						time_16 = 1;
						t16_data = 800;
					} 

					eight_way = 1;

					if(communication_mode == SPI_MODE)
					{
/*CWL 20090310*/		keyboard_tap = toggle_step((signed char)delta_x, (signed char)delta_y);	// detect swipe
					}
					else if(communication_mode == I2C_MODE)
					{
/*CWL 20090310*/		keyboard_tap = toggle_step2((signed char)delta_x, (signed char)delta_y);	// detect swipe
					}


					if(keyboard_tap != 0)
					{

						if(((abs((signed char)delta_x) + abs((signed char)delta_y))) > 20)	//First layer of filter for free scroll mode 
						{
							if(abs((signed char)delta_x) > abs((signed char)delta_y))		//Determines if it's X or Y direction of scroll
							{
								//Codes to filter free scroll in X direction
								if(abs((signed char)delta_x) > 20)							//2nd layer of filter 						
								if((abs(count_x1) +  abs(count_x2) + abs(count_x3))> 30)	//3rd laye of filter
								if(abs(count_x1) > 10)										//4th layer of filter
								if(abs(count_x2) > 20)										//5th layer of filter
								//num_tap = abs((signed char)delta_x);
								num_tap = abs((signed char)X_Acceleration_Buffer);
							}
							else
							{
								//Codes to filter free scroll in Y direction
								if(abs((signed char)delta_y) > 35)							//2nd layer of filter
								if((abs(count_y1) +  abs(count_y2) + abs(count_y3))> 60)	//3rd layer of filter
								if(abs(count_y1) > 30)										//4th layer of filter
								if(abs(count_y2) > 20)										//5th layer of filter
								//num_tap = abs((signed char)delta_y);
								num_tap = abs((signed char)Y_Acceleration_Buffer);
							}

							//num_tap = (abs((signed char)delta_x) + abs((signed char)delta_y));
							if(num_tap != 0)
							if(ofn_mode == ROCKER_M_MODE)
							{
								num_tap = num_tap/10;	
							}
							else
							{
								num_tap = num_tap/2;	
							}	
						}
						else
						{
							num_tap = 0;
							num_tap_count = 0;
						}



						keyboard_dir = keyboard_tap;
						keyboard_tap1 = keyboard_tap;							
					}
					buffer_clear();
					count_z = 0;
				}
				
				if ( (L7 == 1) || (start_continuous_stepping == 1) /*|| (time_16 == 0)*/ )
				{


					if (L7 == 1) 
					{
						time_16 = 1;
						t16_data = 800;
					}

					if ( ( time_16 == 0) || (L7 == 1) && (num_tap == 0) )
					{
						delta_x = delta_y = 0;
						if(communication_mode == SPI_MODE)
						{
//							if (multiple_step_rocker_flag == 0)
/*CWL 20090310*/				keyboard_tap = toggle_step(rocker_delta_x, rocker_delta_y);	// detect swipe
//							else
//								keyboard_tap = toggle_step(rocker_delta_x, rocker_delta_y);

						}
						else if(communication_mode == I2C_MODE)
						{
							keyboard_tap = toggle_step2(rocker_delta_x, rocker_delta_y);	// detect swipe ltlung2009
						}

						if (L7==1) 
						{
							keyboard_tap = 0;
							key_step2 = 0;
							rocker_delta_x = rocker_delta_y = 0;
	 						num_tap = num_tap_count = 0;
						}

						if(keyboard_tap != 0)
						{
							keyboard_dir = keyboard_tap;
							keyboard_tap1 = keyboard_tap;							
						}
						buffer_clear();
						count_z = 0;
						rocker_delta_x = 0;
						rocker_delta_y = 0;
	//					send_rocker_data = 1;
						multiple_step_rocker_flag = 0;
						//start_continuous_stepping = 0;
					}

					else
					{

						t16_data = 800;
						time_16 = 1;
						delta_x = delta_y = 0;

						if(communication_mode == SPI_MODE)
						{
							keyboard_tap = toggle_step((signed char)delta_x, (signed char)delta_y);	// detect swipe
						}
						else if(communication_mode == I2C_MODE)
						{
							keyboard_tap = toggle_step2((signed char)delta_x, (signed char)delta_y);	// detect swipe
						}

						if(keyboard_tap != 0)
						{
							keyboard_dir = keyboard_tap;
							keyboard_tap1 = keyboard_tap;							
						}
						buffer_clear();
						count_z = 0;
					}
				}
				break;			
			
			
			case ROCKER_K_MODE_S_C:

				eight_way = 0;
				delta_x = delta_x * step_multiplier_mode4;
				delta_y = delta_y * step_multiplier_mode4;

				if ( (L7 == 0) && (start_continuous_stepping == 0) )
				{	
					rocker_delta_x=rocker_delta_x+(signed char)delta_x;
					rocker_delta_y=rocker_delta_y+(signed char)delta_y;
				}

				if ( (L7 == 1) || (start_continuous_stepping == 1) )
				{
//					toggle_flag = 1;
				keyboard_tap = toggle_step(rocker_delta_x, rocker_delta_y);	// detect swipe

//CWL 20080310					if ((keyboard_tap == 79) || (keyboard_tap == 80))
//CWL 20080310						keyboard_tap = key_step = key_step2 = 0;
												

					if(keyboard_tap != 0)
					{
						keyboard_dir = keyboard_tap;
						keyboard_tap1 = keyboard_tap;							
					}


					buffer_clear();
					count_z = 0;
					rocker_delta_x = 0;
					rocker_delta_y = 0;
					send_rocker_data = 1;
					//start_continuous_stepping = 0;
				}
				break;


				buffer_clear();
				count_z = 0;
				break;

			case ROCKER_K_MODE_L_C:

				delta_x = delta_x * step_multiplier_mode5;
				delta_y = delta_y * step_multiplier_mode5;

				if ( (L7 == 0) && (start_continuous_stepping == 0) )
				{
					rocker_delta_x=rocker_delta_x+(signed char)delta_x;
					rocker_delta_y=rocker_delta_y+(signed char)delta_y;
						

					keyboard_tap = toggle_step2((signed char)delta_x, (signed char)delta_y);	// detect swipe
					if(keyboard_tap != 0)
					{
						keyboard_dir = keyboard_tap;
						keyboard_tap1 = keyboard_tap;							
					}
					buffer_clear();
					count_z = 0;

				}


				if ( (L7 == 1) || (start_continuous_stepping == 1) )
				{
					delta_x = delta_y = 0;
					if (multiple_step_rocker_flag == 0)
						keyboard_tap = toggle_step2(rocker_delta_x, rocker_delta_y);	// detect swipe
					else
						keyboard_tap = toggle_step(rocker_delta_x, rocker_delta_y);

					if (L7==1)
					{
						keyboard_tap = keyboard_tap1 = keyboard_dir =  0;
						
						key_step2 = 0;
						rocker_delta_x = rocker_delta_y = 0;
 						num_tap = num_tap_count = 0;
					}

					if(keyboard_tap != 0)
					{
						keyboard_dir = keyboard_tap;
						keyboard_tap1 = keyboard_tap;							
					}
					buffer_clear();
					count_z = 0;
					rocker_delta_x = 0;
					rocker_delta_y = 0;
					multiple_step_rocker_flag = 0;
					//start_continuous_stepping = 0;
				}
				break;

			
			/// Modified by TLLAU BEGIN ///
			// Add-in Joystick Mode
			case JOYSTICK_MODE:
				keyboard_tap = joystick_dir(delta_x, delta_y);	// detect swipe
				if(keyboard_tap != 0)
				{
					keyboard_dir = keyboard_tap;
					keyboard_tap1 = keyboard_tap;							
				}
				buffer_clear();
				count_z = 0;
				break;	
			/// Modified by TLLAU END ///

			default:
				break;
		}
	}
}

unsigned char joystick_dir(signed char deltaX, signed char deltaY)
{
	unsigned char joystick_step;
	float atan_temp;

	joystick_step = 0;

	if((time_0 == 0) && (toggle_flag == 0))
	{
		toggle_flag = 1;
		deltaX = 0;
		deltaY = 0;
		X_Total_Displacement = 0;
		Y_Total_Displacement = 0;
		t0_data = 30;	
		time_0 = 1;
	}

	if(toggle_flag == 1)
	{
		if(time_0 == 1)
		{
				X_Total_Displacement += (signed char) deltaX;
				Y_Total_Displacement += (signed char) deltaY;
		}
		else if(time_0 == 0)
		{
			toggle_flag = 0;
			
			X_Total_Displacement += (signed char) deltaX;
			Y_Total_Displacement += (signed char) deltaY;
						
			if((abs(X_Total_Displacement) + abs(Y_Total_Displacement)) < 10)	
			{
				X_Total_Displacement = 0;
				Y_Total_Displacement = 0;
				deltaX = 0;
				deltaY = 0;
			}
			else

			{
				X_Total_Displacement += (signed char) deltaX;
				Y_Total_Displacement += (signed char) deltaY;

				atan_temp = atan2(Y_Total_Displacement, X_Total_Displacement + 10);

				if(atan_temp < 0)
					atan_temp += 2 * PI;

				// 337.5 deg 	-> 022.5 deg ==> RIGHT
				// 022.5 deg	-> 067.5 deg ==> UP-RIGHT
				// 067.5 deg 	-> 112.5 deg ==> UP
				// 112.5 deg 	-> 157.5 deg ==> UP-LEFT
				// 157.5 deg 	-> 202.5 deg ==> LEFT
				// 202.5 deg 	-> 247.5 deg ==> DOWN-LEFT
				// 247.5 deg 	-> 292.5 deg ==> DOWN
				// 292.5 deg 	-> 337.5 deg ==> DOWN-RIGHT

				if((atan_temp < 0.3927) || (atan_temp > 5.8905))
				{
					joystick_step = RIGHT_KEY;
				}
				else if((atan_temp > 0.3927) && (atan_temp < 1.1781))
				{
					joystick_step = UP_R_KEY;
				}
				else if((atan_temp > 1.1781) && (atan_temp < 1.9635))
				{
					joystick_step = UP_KEY;
				}
				else if((atan_temp > 1.9635) && (atan_temp < 2.7489))
				{
					joystick_step = UP_L_KEY;
				}
				else if((atan_temp > 2.7489) && (atan_temp < 3.5343))
				{
					joystick_step = LEFT_KEY;
				}
				else if((atan_temp > 3.5343) && (atan_temp < 4.3197))
				{
					joystick_step = DOWN_L_KEY;
				}
				else if((atan_temp > 4.3197) && (atan_temp < 5.1051))
				{
					joystick_step = DOWN_KEY;
				}
				else if((atan_temp > 5.1051) && (atan_temp < 5.8905))
				{
					joystick_step = DOWN_R_KEY;
				}

				X_Total_Displacement = 0;
				Y_Total_Displacement = 0;
				deltaX = 0;
				deltaY = 0;
				t0_data = 50;	
				time_0 = 1;	
			}
		}
	}

	return joystick_step;
}	




