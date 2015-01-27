

//;***************************************************************************
//;
//;        Copyright 2007 Avago Technologies Limited
//;    This code is provided by Avago Technologies as a reference.
//;     Avago Technologies makes no claims or warranties
//;    to this firmware's suitability for any application.
//;    All rights reserved. Patent Pending. 
//;
//;*************************************************************************** 

extern unsigned int data t0_data, t1_data, t2_data, t3_data, t4_data, t5_data, t6_data, t7_data, t8_data, t9_data;
extern bit time_0, time_1, time_2, time_3, time_4, time_5, time_6, time_7, time_8, time_9;

extern unsigned int t10_data, t11_data, t12_data, t13_data, t14_data, t15_data, t16_data;
extern bit time_10, time_11, time_12, time_13, time_14, time_15, time_16;

extern unsigned int t99_data;
extern bit time_99;

extern void Timer0Init (void);

extern unsigned int data tkeypad_data;
extern bit bdata time_keypad;

extern unsigned int data tlcd_data;
extern bit bdata time_lcd;

extern unsigned int data ps2_0_data, ps2_1_data;
extern bit bdata ps2_time_0, ps2_time_1;

extern unsigned int data t_toggle_data;
extern bit bdata time_toggle;

extern unsigned int data t_temp_data;
extern bit bdata time_temp;

extern unsigned int data t_calibrate_data, t_left_data, t_right_data, t_click_data;
extern bit bdata time_calibrate, time_left, time_right, time_click;

extern unsigned int t_continuous_rock_trigger_data, t_continuous_rock_interval_data;
extern bit t_continuous_rock_trigger, t_continuous_rock_interval;