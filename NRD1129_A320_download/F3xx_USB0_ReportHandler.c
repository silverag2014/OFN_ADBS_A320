//-----------------------------------------------------------------------------
// F3xx_USB0_ReportHandler.c
//-----------------------------------------------------------------------------
// Copyright 2005 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Contains functions and variables dealing with Input and Output
// HID reports.
//
// How To Test:    See Readme.txt
//
//
// FID:            3XX000038
// Target:         C8051F340
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
//                 Silicon Laboratories IDE version 2.6
// Command Line:   See Readme.txt
// Project Name:   F3xx_MouseExample
//
// Release 1.1
//    -Minor code comment changes
//    -16 NOV 2006
// Release 1.0
//    -Initial Revision (PD)
//    -07 DEC 2005
//


// ----------------------------------------------------------------------------
// Header files
// ----------------------------------------------------------------------------

#include "F3xx_USB0_ReportHandler.h"
#include "F3xx_USB0_InterruptServiceRoutine.h"
#include "F3xx_USB0_Mouse.h"


// ----------------------------------------------------------------------------
// Local Function Prototypes
// ----------------------------------------------------------------------------

// ****************************************************************************
// Add custom Report Handler Prototypes Here
// ****************************************************************************

void IN_Report(void);
void OUT_Report(void);

// ----------------------------------------------------------------------------
// Local Definitions
// ----------------------------------------------------------------------------

// ****************************************************************************
// Set Definitions to sizes corresponding to the number of reports
// ****************************************************************************

#define IN_VECTORTABLESize 1
#define OUT_VECTORTABLESize 1

// ----------------------------------------------------------------------------
// Global Constant Declaration
// ----------------------------------------------------------------------------


// ****************************************************************************
// Link all Report Handler functions to corresponding Report IDs
// ****************************************************************************

const VectorTableEntry code IN_VECTORTABLE[IN_VECTORTABLESize] =
{
   // FORMAT: Report ID, Report Handler
   0, IN_Report
};

// ****************************************************************************
// Link all Report Handler functions to corresponding Report IDs
// ****************************************************************************
const VectorTableEntry code OUT_VECTORTABLE[OUT_VECTORTABLESize] =
{
   // FORMAT: Report ID, Report Handler
   0, OUT_Report
};


// ----------------------------------------------------------------------------
// Global Variable Declaration
// ----------------------------------------------------------------------------

BufferStructure IN_BUFFER, OUT_BUFFER;

// ----------------------------------------------------------------------------
// Local Functions
// ----------------------------------------------------------------------------

// ****************************************************************************
// Add all Report Handler routines here
// ****************************************************************************


// ****************************************************************************
// For Input Reports:
// Point IN_BUFFER.Ptr to the buffer containing the report
// Set IN_BUFFER.Length to the number of bytes that will be
// transmitted.
//
// REMINDER:
// Systems using more than one report must define Report IDs
// for each report.  These Report IDs must be included as the first
// byte of an IN report.
// Systems with Report IDs should set the FIRST byte of their buffer
// to the value for the Report ID
// AND
// must transmit Report Size + 1 to include the full report PLUS
// the Report ID.
//
// ****************************************************************************


void IN_Report(void){

	IN_PACKET[0] = 0;	//Report ID
	IN_PACKET[1] = 0;	//Buttons
	IN_PACKET[2] = 0;	//X-Axis High Byte
	IN_PACKET[3] = 0;	//X-Axis Low Byte
	IN_PACKET[4] = 0;	//Y-Axis High Byte
	IN_PACKET[5] = 0;	//Y-Axis Low Byte
	IN_PACKET[6] = 0;	//Z-Axis

   // point IN_BUFFER pointer to data packet and set
   // IN_BUFFER length to transmit correct report size
}


// ****************************************************************************
// For Output Reports:
// Data contained in the buffer OUT_BUFFER.Ptr will not be
// preserved after the Report Handler exits.
// Any data that needs to be preserved should be copyed from
// the OUT_BUFFER.Ptr and into other user-defined memory.
//
// ****************************************************************************

void OUT_Report(void)
{
}

