// lcd_test.c
#define __LPC2148

#include "../mainLPCheader.h"

#include "../types.h"
#include "../delay.h"
#include "../defines.h"
#include "../lcd_defines.h"
#include "../lcd.h"

uint8 cgramLUT[][8] = 
{
	{0x00,0x01,0x03,0x07,0x0F,0x1F,0x3F,0x00},
	{0x04,0x0E,0x1F,0x0E,0x04,0x05,0x06,0x00},
	{0x04,0x0E,0x1F,0x0E,0x04,0x14,0x1C,0x00},
};

int main()
{
	// Initialize LCD
	InitLCD();
	// BuildCGRAM
	BuildCGRAMLUT(cgramLUT, 3);
	CmdLCD(GOTO_LINE1_POS0);
	StrLCD("***LCD TEST***");
	CmdLCD(GOTO_LINE2_POS0);
	delay_s(1);
	StrLCD("NUMBER: ");
	S32LCD(-69);
	delay_ms(500);
	CmdLCD(GOTO_LINE2_POS0);
	StrLCD("REAL: ");
	F32LCD(-963.4569871, 5);
	delay_s(1);
	CmdLCD(GOTO_LINE2_POS0);
	StrLCD("                ");
	CmdLCD(GOTO_LINE2_POS0);	
	
	delay_s(1);
	CmdLCD(GOTO_LINE2_POS0);
	StrLCD("Symbols: ");
	CharLCD(0);
	delay_s(1);
	CharLCD(1);
	delay_s(1);
	CharLCD(2);
	while(1);
}
