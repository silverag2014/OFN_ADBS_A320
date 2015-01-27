
//;***************************************************************************
//;
//;        Copyright 2007 Avago Technologies Limited
//;    This code is provided by Avago Technologies as a reference.
//;     Avago Technologies makes no claims or warranties
//;    to this firmware's suitability for any application.
//;    All rights reserved. Patent Pending. 
//;
//;*************************************************************************** 


#define ENTER_KEY		0x28
#define UP_KEY			0x52
#define DOWN_KEY		0x51
#define RIGHT_KEY		0x4F
#define LEFT_KEY		0x50

#define INSERT			0x49
#define BACKSPACE		0x2A

extern unsigned char left_state, right_state;
extern int eight_way; // Modified by TLLAU

extern void Shutter_Calibration(void);
extern void Tap_Detection(void);
extern void Click_Action(void);
extern void Right_Detection(void);
extern void speed_switching(void);
extern void buffer_clear(void);
extern unsigned char toggle_step(int X_Total_Displacement, int Y_Total_Displacement);
extern unsigned char key_step, key_step2, finger_status;
extern int X_Acceleration_Buffer, Y_Acceleration_Buffer;
