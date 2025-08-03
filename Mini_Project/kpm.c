// kpm.c

#include "MCU_settings.h"
#include "mainLPCheader.h"
#include "types.h"
#include "delay.h"
#include "defines.h"
#include "lcd.h"
#include "kpm_defines.h"

// Password enter KPM LUT
const uint8 passwordLUT[4][4] = 
{
	{'1', '2', '3', 'B'},
	{'4', '5', '6', 'C'},
	{'7', '8', '9', ' '},
	{'*', '0', '#', 'E'}
};

void InitKPM(void)
{
	// Initialize KPM
	
	// Rows as output, Cols as Input
	WRITENIBBLE(IODIR1, ROW0, 0xF);
	// Cols as Input by-default
}

uint32 colScan(void)
{
	// Col Scan
	// Check all the cols are set to 1
	return (READNIBBLE(IOPIN1, COL0) == 15);
}

uint32 rowCheck(void)
{
	// If ColScan detected any key-press, 
	// Check for which row the switch is pressed
	uint32 r;
	for (r=0; r<4; r++)
	{
		WRITENIBBLE(IOPIN1, ROW0, ~(1<<r));
		if (colScan() == 0)
			break;
	}
	// reset the Rows
	WRITENIBBLE(IOPIN1, ROW0, 0);
	//IOCLR1 = (1<<ROW0) | (1<<ROW1) | (1<<ROW2) | (1<<ROW3);
	return r;
}

uint32 colCheck(void)
{
	// If ColScan detected any key-press,
	// Check for which col the switch is pressed
	uint32 c;
	for (c=0; c<4; c++)
	{
		if (READBIT(IOPIN1, (COL0+c)) == 0)
			break;
	}
	return c;
}

uint8 keyScan(void)
{
	// Key Scan
	uint32 row, col;
	// Check if there is any keypress
	while(colScan());
	// if key press detected
	// get row
	row = rowCheck();
	// get col
	col = colCheck();
	
	return passwordLUT[row][col];
}


// User defined function to get input from KPM
uint32 getU32InKPM(void)
{
	// Function to scan positive numeric data through KPM.
	// Only numeric keys entered are considered as data except for special symbols like '*', '#', 'SPACE'
	// If key entered is in the followinf 'B', 'C', 'E': these are special characters which has their own meaning and has special functionality
	uint32 res=0;
	uint8 key;
	
	while (res < 10000)
	{
		key = keyScan();
		
		if (key == 'E')
			break;
		
		switch(key)
		{	
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9': res = (res*10) +  (key-'0');
					CharLCD(key);
					break;
			case ' ': // space is invalid, don't consider as a key
			case '*': // '*' is invalid, don't consider as a key
			case '#': // '#' is invalid, don't consider as a key
						break;
			case 'B': // backspace
					res = res/10;
					// clear part of cursor
					CmdLCD(SHIFT_CUR_LEFT);
					CharLCD(' ');
					CmdLCD(SHIFT_CUR_LEFT);
						break;
			case 'C': // clear entered value
					while (res != 0)
					{
						res = res/10;
						// clear part of cursor
						CmdLCD(SHIFT_CUR_LEFT);
						CharLCD(' ');
						CmdLCD(SHIFT_CUR_LEFT);
					}
					break;
				
		}
		delay_ms(250);
	}
	return res;
}
