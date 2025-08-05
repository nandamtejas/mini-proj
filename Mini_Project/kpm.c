// kpm.c

#include "MCU_settings.h"
#include "mainLPCheader.h"
#include "types.h"
#include "delay.h"
#include "defines.h"
#include "lcd.h"
#include "kpm_defines.h"

// extern variable to hide the password
extern volatile uint32 hide;

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

// Function to get string input from KPM
void strKeyScan(uint8* result)
{
	// This function is to scan the string input from KPM and return the entered string
	//uint8 result[16];
	uint8 key;
	int32 i=0;
	
	while(1)
	{
		
		// break if the entered string length exceeds 16
		if (i>=16)
			break;
		
		// call the keyScan function
		key = keyScan();
		delay_ms(150);
		
		// if key == 'E', break the loop
		if (key == 'E')
		{
			result[i] = '\0';
			break;
		}
		
		// Check for the conditions
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
			case '9':
			case '*':
			case '#':
			case ' ': result[i++] = key;
						CharLCD(key);
						delay_ms(50);
						if (hide==1)
						{
							// clear part of cursor
							CmdLCD(SHIFT_CUR_LEFT);
							CharLCD(' ');
							CmdLCD(SHIFT_CUR_LEFT);
							// display '*' instead of entered key
							CharLCD('*');
						}
						break;
			case 'B': // backspace
					if (i == 0)
						// if the i=0, i.e; the cursor is at starting position, we should not alter the cursor
						break;
					result[i--] = '\0';
					// clear part of cursor
					CmdLCD(SHIFT_CUR_LEFT);
					CharLCD(' ');
					CmdLCD(SHIFT_CUR_LEFT);
						break;
			case 'C': // clear password
					for (--i; i>=0; i--)
					{
						result[i] = '\0';
						// clear part of cursor
						CmdLCD(SHIFT_CUR_LEFT);
						CharLCD(' ');
						CmdLCD(SHIFT_CUR_LEFT);
					}
					break;
		}
	}
}

// User defined function to get input from KPM
uint32 U32KeyScan(void)
{
	// Function to scan positive numeric data through KPM.
	uint32 res=0;
	uint8 i;
	uint8 resStr[5];
	
	// Call strKeyScan function to enter the value
	strKeyScan(resStr);
	
	// Convert the entered input to an unsigned integer
	for (i=0; resStr[i] != '\0'; i++)
	{
		if (ISWITHIN(resStr[i], '0', '9'))
		{
			res = (res*10) + (resStr[i]-'0');
		}
	}
	return res;
}
