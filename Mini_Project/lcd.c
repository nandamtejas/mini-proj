// lcd.c
/*
LCD related functionality is included in this file
*/
#include "MCU_settings.h"
#include "mainLPCheader.h"

#include "types.h"
#include "defines.h"
#include "lcd_defines.h"
#include "delay.h"


void WriteLCD(uint8 byte)
{
	// Function to Write a byte in LCD
	
	// Write data into LCD
	WRITEBYTE(IOPIN0, LCD_DATA, byte);
	
	// to write into LCD, the RW should be grounded
	// WRITEBIT(IOPIN0, LCD_RW, 0);
	IOCLR0 = 1<<LCD_RW;
	
	// after writing the data byte into LCD, we need to trigger Enable pin in Falling Edge @1us
	//	then wait for 1.44ms for completing the operation
	IOSET0 = 1<<LCD_EN;
	delay_us(1);
	IOCLR0 = 1<<LCD_EN;
	delay_ms(5);
}

void CmdLCD(uint8 cmd)
{
	// Function to write a command in LCD
	
	// To write command into LCD, LCD_RS register should be CLEARED
	IOCLR0 = 1<<LCD_RS;
	WriteLCD(cmd);
}

void InitLCD(void)
{
	// LCD Initialization
	// 1. If 5V power-supply is is given, wait for 15ms
	//    else if 3V power-supply is given, wait for 40ms
	/*** WE ASSUME 5V POWER_SUPPY IS GIVEN TO LCD VDD PIN ***/
	delay_ms(15);
	
	// We set the LCD_DATA pins as gpio output pins
	WRITEBYTE(IODIR0, LCD_DATA, 0xFF);
	WRITEBYTE(IODIR0, LCD_RS, 7);
	// we use by-default 8-BIT mode
	// The initialization steps as follows
	
	// 2. Function set to 8-bit 1-line mode and wait for 4.1ms
	CmdLCD(0x30);
	delay_ms(4);
	delay_us(100);
	
	// 3. Repeat the same as above and wait for 100us
	CmdLCD(0x30);
	delay_us(100);
	
	// 4. Repeat the same
	CmdLCD(0x30);
	
	// LCD Connection is Established in 8-bit mode, now we can set to the user desired
	
	// 5. For 8-bit 2-line mode, we need to function set to desired value
	CmdLCD(MODE_8BIT_2LINE);
	
	// Turn of the display, with underlined cursor not blinking
	CmdLCD(DSP_ON_CUR_NOBLINK);
	
	// Clear the display
	CmdLCD(CLEAR_LCD);
	
	// Entry mode set
	// We need the cursor to auto-increment and should shift towards right after increment
	CmdLCD(ENTRY_CUR_RIGHT);
	CmdLCD(GOTO_LINE1_POS0);
}

void CharLCD(uint8 asciiVal)
{
	// Function to write Character data in LCD
	// To write Data, LCD_RS register should be SET
	IOSET0 = 1<<LCD_RS;
	WriteLCD(asciiVal);
}

void StrLCD(uint8 *p)
{
	// Function to write data array of Character (can be called as String) in LCD
	// Here every character in string is written in LCD by calling CharLCD function
	while(*p)
	{
		CharLCD(*p);
		p++;
	}
}

void U32LCD(uint32 num)
{
	// Function to write positive numeric data in LCD
	
	// Step 1: Convert the numeric data to string (itoa logic)
	uint8 a[10];
	int32 i=0;
	
	while (num != 0)
	{
		a[i++] = num%10 + '0';
		num = num/10;
	}
	
	// Step 2: Write the string in reverse order
	for (--i; i>=0; i--)
	{
		CharLCD(a[i]);
	}	
}
	
void S32LCD(int32 num)
{
	// Function to write positive or negative numeric data in LCD
	// If num < 0, represent '-'
	if (num < 0)
	{
		CharLCD('-');
		num = -num;
	}
	U32LCD(num);
}

void F32LCD(real32 fnum, uint32 precision)
{
	// Function to write real/float data in LCD
	uint32 num, i;
	
	// check for -ve numbers
	if (fnum < 0)
	{
		CharLCD('-');
		fnum = -fnum;
	}
	num = fnum;
	U32LCD(num);
	CharLCD('.');

	// now display number of digits after decimal point
	for (i=0; i<precision; i++)
	{
		fnum = (fnum-num)*10;
		num = fnum;
		CharLCD(num+'0');
	}
}

void BuildCGRAM(const uint8 *p, uint32 bytes)
{
	/*
	This function is to create custom symbols to be written in CGRAM of LCD
	**NOTE: This function should be used in setup mode**
	
	Inputs:
		- *p -> represents array of custom bytes
		- bytes -> no of bytes to be written in CGRAM
	*/
	
	uint32 i;
	// This function is to create custom symbols to be written in CGRAM of LCD
	// for that, the LCD cursor which is pointing to DDRAM memory (display),  should point to the CGRAM start pos
	
	CmdLCD(GOTO_CGRAM_START);
	// Now the cursor points to CGRAM memory
	
	// Then enable the LCD in command mode by setting the RS register 
	IOSET0 = 1<<LCD_RS;
	
	// for write the bytes in CGRAM memory
	for (i=0; i<bytes; i++)
	{
		WriteLCD(p[i]);
	}
	
	// now return the cursor pointer to DDRAM display
	CmdLCD(GOTO_LINE1_POS0);
}

void BuildCGRAM_v2(const uint8 *p, uint32 index)
{
	/*
	Version 2
	This function is to create custom symbols to be written in CGRAM of LCD
	**NOTE: This function should be used in setup mode**
	
	Inputs:
		- *p -> array of 8 bytes
		- index -> CGRAM index
	*/
	uint32 i;
	
	// The LCD cursor which is pointing to DDRAM memory (display),  should point to the desired index position of CGRAM memory
	
	CmdLCD(GOTO_CGRAM_START+(8*index));
	
	// Then enable the LCD in command mode by clearing the RS register 
	IOSET0 = 1<<LCD_RS;
	
	// for write the bytes in CGRAM memory
	for (i=0; i<8; i++)
	{
		WriteLCD(p[i]);
	}
	
	// now return the cursor pointer to DDRAM display
	CmdLCD(GOTO_LINE1_POS0);
}

void BuildCGRAMLUT(const uint8 (*p)[8], uint32 nElem)
{
	/*
	This function is to create custom symbols to be written in CGRAM of LCD
	**NOTE: This function should be used in setup mode**
	
	Inputs:
		- *p -> pointer to an array of 8 elements
		- nElem -> no of elements / symbols (8 bytes) to write in CGRAM memory 
	*/
	uint32 i;
	
	// call BuildCGRAM_v2 to write into CGRAM
	for (i=0; i<nElem; i++)
	{
		BuildCGRAM_v2(p[i], i);
	}
}
