// kpm_admin.c

#include <string.h>
#include "MCU_settings.h"
#include "mainLPCheader.h"
#include "types.h"
#include "delay.h"
#include "defines.h"
#include "lcd.h"
#include "kpm.h"
#include "rtc.h"
#include "messages.h"


// function to enter the password and returns the entered password
void enter_password(int8* password)
{
	// function to start password typing activity
	int32 i=0;
	uint8 key;
	
	while (1)
	{
		// scan every key entered is a valid password key or special key
		// if numeric or symbol key listed in switch case entered, consider it as a part of password
		// if special keys entered such as 'B', 'C', 'E', perform the corresponding activity
		key=keyScan();
		
		if (i >= 16)
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
			case '9':
			case '*':
			case '#':
			case ' ': password[i++] = key;
						CharLCD(key);
						delay_ms(50);
						// clear part of cursor
						CmdLCD(SHIFT_CUR_LEFT);
						CharLCD(' ');
						CmdLCD(SHIFT_CUR_LEFT);
						// display '*' instead of entered key
						CharLCD('*');
						break;
			case 'B': // backspace
					password[i--] = '\0';
					// clear part of cursor
					CmdLCD(SHIFT_CUR_LEFT);
					CharLCD(' ');
					CmdLCD(SHIFT_CUR_LEFT);
						break;
			case 'C': // clear password
					for (--i; i>=0; i--)
					{
						password[i] = '\0';
						// clear part of cursor
						CmdLCD(SHIFT_CUR_LEFT);
						CharLCD(' ');
						CmdLCD(SHIFT_CUR_LEFT);
					}
					break;
			case 'E': // Exit the function
					password[i] = '\0';
					return;
		}
		delay_ms(250);
	}
}

// function to verify password
uint32 verify_password(int8* password)
{
	// strcmp logic
	return !(strcmp(ADMIN_PASSWORD, password));
}

void clearPassword(int8* password)
{
	// function clears the password stored in memory
	int32 len=0;
	while (password[len++] != '\0');
	
	for (--len; len>=0; len--)
	{
		password[len] = '\0';
	}
}	

// admin_enter_password
void admin_enter_password()
{
	// ADMIN mode activity starts
	int8 password[20];
	
	// Password Activity displayed in LCD starts
PASSW:	CmdLCD(DSP_ON_CUR_NOBLINK);
	CmdLCD(CLEAR_LCD);
	CmdLCD(GOTO_LINE1_POS0);
	StrLCD(" ENTER PASSWORD ");
	
	CmdLCD(GOTO_LINE2_POS0);
	
	// call enter_password function
	enter_password(password);
	delay_ms(250);
	
	// Clear screen after pasword entered
	CmdLCD(CLEAR_LCD);
	CmdLCD(DSP_ON_CUR_OFF);
	
	if (verify_password(password))
	{
		// if correct password entered, goto ADMIN mode	
		StrLCD("WELCOME ADMIN");
	}
	else
	{
		// if wrong password entered, try again
		StrLCD(" WRONG PASSWORD ");
		CmdLCD(GOTO_LINE2_POS0);
		StrLCD("TRY AGAIN!");
		// clear the password in memory and try password activity again
		clearPassword(password);
		delay_ms(750);
		goto PASSW;
	}
	delay_ms(750);	
}

// Admin mode
void adminMode(void)
{
	// Admin mode function
	
	// Functionality to modify the Date and Time or Message enabled
	uint8 key;
	CmdLCD(CLEAR_LCD);
	CmdLCD(DSP_ON_CUR_OFF);
	
	// Admin Mode Starts
	StrLCD("ADMIN MODE");
	delay_ms(180);
	
	// Admin Authentication
	admin_enter_password();
	delay_ms(250);
	
	// Admin Mode Activity starts after Admin Authentication
AFT_PASS: CmdLCD(DSP_ON_CUR_OFF);
	CmdLCD(CLEAR_LCD);
	StrLCD("1. Edit DATE/TIME");
	CmdLCD(GOTO_LINE2_POS0);
	StrLCD("2. Edit MSG");
	
	// Enter the valid options
	key = keyScan();
	delay_ms(150);
	switch(key)
	{
		case '1': // RTC time or date edit
			//edit_time();
			edit_date_or_time();
			break;
		case '2': // MSG enable edit
			edit_message();
			break;
		default: // other than '1' or '2' key is pressed, take the input once again
			CmdLCD(CLEAR_LCD);
			StrLCD("Wrong key pressed!");
			CmdLCD(GOTO_LINE2_POS0);
			StrLCD("Press 1 or 2");
			delay_ms(500);
			goto AFT_PASS;
	}
	CmdLCD(CLEAR_LCD);
	
	// select continue or exit
	StrLCD("1. Continue");
	CmdLCD(GOTO_LINE2_POS0);
	StrLCD("Any key to Exit");
	key = keyScan();
	delay_ms(150);
	switch(key)
	{
		case '1': // continue
			goto AFT_PASS;
		default: // press any key to exit
			break;
	}
}



