//-----------------------------------------------------------------------------
// F3xx_USB_Main.c
//-----------------------------------------------------------------------------
// Copyright 2005 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This application will communicate with a PC across the USB interface.
// The device will appear to be a mouse, and will manipulate the cursor
// on screen.
//
// How To Test:    See Readme.txt
//
//
// FID:            3XX000035
// Target:         C8051F32x/C8051F340
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
//                 Silicon Laboratories IDE version 2.6
// Command Line:   See Readme.txt
// Project Name:   F3xx_MouseExample
//
//
// Release 1.0
//
// Initial revision.
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "c8051f3xx.h"
#include "F3xx_USB0_InterruptServiceRoutine.h"
#include "F3xx_USB0_Mouse.h"
#include "timer.h"

extern void usbTaskLoop(void);
//-----------------------------------------------------------------------------
// Main Routine
//-----------------------------------------------------------------------------
void main(void)
{
   System_Init ();

   EA = 1;

   usbTaskLoop();
}

