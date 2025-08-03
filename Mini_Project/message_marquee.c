// message_marquee.c

// required headers
#include "MCU_settings.h"
#include "mainLPCheader.h"
#include "types.h"
#include "delay.h"
#include "defines.h"
#include "lcd.h"

// As we are using 16x2 LCD to display where LCD has 2 lines and each line can hold 16 characters each, hence the WINDOW size if 16
#define WINDOW 16

int32 i,j,k,c;
int32 cursorPos;
uint8 lcd_msg[WINDOW*4];

// External declaration of scrollStopFlag variable
// This variable is defined in main.c and shared across multiple source files
// It is declared as 'volatile' to reflect that its value may change unexpectedly (e.g., in ISRs or other threads)
// Do not define it here — this is just a declaration to access the same global instance
extern volatile uint32 scrollStopFlag;


void message_marquee(uint8* msg)
{
	// message marquee
	// the whole message is scrolled from right to left one time
	// for continuous scrolling, need to call this function inside while loop
	
	i=0,j=0, k=0, c=0;
	cursorPos = WINDOW-1;

	for (i=0; msg[k]!='\0'; i++)
	{
		// Clear the displayed message initially on LCD display line
		//CmdLCD(GOTO_LINE1_POS0);
		//StrLCD("                ");
		
		// now modify the lcd_msg to display the message in LCD
		j=k;
		for (c=cursorPos; c<WINDOW; c++)
		{
			// if c pointer is less than the initial cursorPos, modify lcd_msg[c] as ' ', else with the character of index j is pointing
			CmdLCD(GOTO_LINE1_POS0+c);
			if ((msg[j] == '\0'))
				CharLCD(' ');
			else
				CharLCD(msg[j++]);
		}
		// decrement the cursorPos as it is the pointer responsible for flow of marquee from right to left
		if (--cursorPos<0)
		{
			cursorPos = 0;
			k++;
		}
		// wait for some time for displaying
		delay_ms(50);
	}
}

void message_marquee_v2(uint8 *msg)
{
	// This is a message_marquee version 2 function, where the logic is slightly modified.
	// In previous version, we iterate through WINDOW times to display the message untill the k value reaches it's limit (i.e; msg[k] == '\0' -> loop terminates
	// In this version, instead of using loops, LCD Shift display left command is used
	// Initially all the spaces are padded till the rightmost end of LCD and Write the message
	// and then shift the message towards left.
	
	uint8 msg_len=0,i;
	// find the msg_len
	while(msg[msg_len++]!='\0');
	
	// now padd the display with spaces from LCD start position
	CmdLCD(GOTO_LINE1_POS0);
	for(i=0; i<WINDOW-1; i++)
	{
		CharLCD(' ');
	}
	// write the msg to the display
	StrLCD(msg);
	
	// Now shift the display towards right
	for (i=0; i<msg_len+WINDOW-1; i++)
	{
		CmdLCD(SHIFT_DSP_LEFT);
		delay_ms(150);
	}
}

void message_marquee_v3(uint8* msg)
{
	// message marquee version 3
	// the whole message is scrolled from right to left one time
	// for continuous scrolling, need to call this function inside while loop
	// This functionality can be terminated through the scrollStopFlag condition which is externally declared in main.c
	
	// scrollStopFlag=0 -> continue the process
	// scrollStopFlag=1 -> Terminate the process
	
	i=0,j=0, k=0, c=0;
	cursorPos = WINDOW-1;
	
	// To realize the marquee, we need to check two condition as below mentioned
	for (i=0; ((msg[k]!='\0') && (scrollStopFlag==0)); i++)
	{
		
		// now modify the lcd_msg to display the message in LCD
		j=k;
		for (c=0; c<WINDOW; c++)
		{
			// If c is less than current cursorPos or message ends, print space
			// Otherwise, print characters starting from msg[j]
			if ((c<cursorPos) || (msg[j] == '\0'))
				lcd_msg[c] = ' ';
			else
				lcd_msg[c] = msg[j++];
		}
		// Move the message to the left by decreasing cursor position
		// Once cursorPos reaches 0, increment starting index k

		if (--cursorPos<0)
		{
			cursorPos = 0;
			k++;
		}
		CmdLCD(GOTO_LINE1_POS0);
		StrLCD(lcd_msg);
		// wait for some time for displaying
		delay_ms(50);
	}
}
