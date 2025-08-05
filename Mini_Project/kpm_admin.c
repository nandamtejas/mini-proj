// kpm_admin.c

/*
This file is solely for ADMIN mode, and ADMIN mode is mostly realized through KPM
*/

#include "MCU_settings.h"
#include "mainLPCheader.h"
#include "types.h"
#include "delay.h"
#include "defines.h"
#include "lcd.h"
#include "kpm.h"
#include "rtc.h"
#include "messages.h"

// strcmp prototype
int32 strcmp(const char * , const char *);

// variable to hide the password
volatile uint32 hide=0;

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
	uint8 password[20];
	
	// Password Activity displayed in LCD starts
PASSW:	CmdLCD(DSP_ON_CUR_NOBLINK);
	CmdLCD(CLEAR_LCD);
	CmdLCD(GOTO_LINE1_POS0);
	StrLCD(" ENTER PASSWORD ");
	
	CmdLCD(GOTO_LINE2_POS0);
	
	// call enter_password function
	//enter_password(password);
	hide=1;
	strKeyScan(password);
	hide=0;
	delay_ms(250);
	
	// Clear screen after pasword entered
	CmdLCD(CLEAR_LCD);
	CmdLCD(DSP_ON_CUR_OFF);
	
	if (verify_password((int8*)password))
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
		clearPassword((int8*)password);
		delay_ms(750);
		goto PASSW;
	}
	delay_ms(750);	
}

// Admin mode
void adminMode(void)
{
	// Admin mode function, called when EINT0 interrupt occurs
	
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
	delay_ms(50);
	// select continue or exit
	StrLCD("1. Continue");
	CmdLCD(GOTO_LINE2_POS0);
	StrLCD("Any key to Exit");
	
	key = keyScan();
	delay_ms(100);
	switch(key)
	{
		case '1': // continue
			goto AFT_PASS;
		default: // press any key to exit
			break;
	}
}



