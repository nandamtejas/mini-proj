// rtc.c

#include "rtc_defines.h"
#include "MCU_settings.h"
#include "mainLPCheader.h"
#include "types.h"
#include "delay.h"
#include "defines.h"
#include "lcd.h"
#include "kpm.h"


// Digital clock global variables
// uint32 date=0, month=0, year=0;
// uint32 hour=0, min=0, sec=0;

// WEEK enum table
typedef enum {SUN, MON, TUE, WED, THU, FRI, SAT} WEEK;


void InitRTC()
{
	// This function is for RTC initialisation
	
	// Disable clock and reset before configuration
    CCR = RTC_RESET;
	
	#if (RTC_CLKSRC == 0)
		PREINT = PREINT_FACT;
		PREFRAC = PREFRAC_FACT;
	#endif
	
	
	#if (defined(__LPC2148) && (RTC_CLKSRC == 1))
			CCR = RTC_ENABLE | RTC_SRC;
	#else
		CCR = RTC_ENABLE;
	#endif
		
}

// Date and time
void setTime(uint32 hour, uint32 min, uint32 sec)
{
	// This function is used to set time in RTC SFR's
	SEC = sec;
	MIN = min;
	HOUR = hour;
}

void getTime(uint32 *hour, uint32 *min, uint32 *sec)
{
	// This function is used to get time from RTC SFR's
	*sec = SEC;
	*min = MIN;
	*hour = HOUR;
}

void setDate(uint32 date, uint32 month, uint32 year)
{
	// This function is used to set date in RTC SFR's
	DOM = date;
	MONTH = month;
	YEAR = year;
}

void getDate(uint32 *date, uint32 *month, uint32 *year)
{
	// This function is used to get date from RTC SFR's
	*date = DOM;
	*month = MONTH;
	*year = YEAR;
}
	
// Alarm
void setAlarmTime(uint32 hour, uint32 min)
{
	// This function is used to set Alarm
	// NOTE: Make sure to Enable RTCALF_BIT bit in ILR register and RTC_ALARM_EN to 1
	// The Alarm is set to hour and min
	ALMIN = min;
	ALHOUR = hour;
}


// display time function
void displayClock(void)
{
	// Function to display time
	
	// Show time
	CmdLCD(GOTO_LINE1_POS0);
	CharLCD((HOUR/10) + '0');
	CharLCD((HOUR%10) + '0');
	CharLCD(':');
	CharLCD((MIN/10) + '0');
	CharLCD((MIN%10) + '0');
	CharLCD(':');
	CharLCD((SEC/10) + '0');
	CharLCD((SEC%10) + '0');
	
	// Show WeekDay
	switch(DOW)
	{
		case SUN: StrLCD(" SUN"); break;
		case MON: StrLCD(" MON"); break;
		case TUE: StrLCD(" TUE"); break;
		case WED: StrLCD(" WED"); break;
		case THU: StrLCD(" THU"); break;
		case FRI: StrLCD(" FRI"); break;
		case SAT: StrLCD(" SAT"); break;
	}
	
	
	// Show Date
	CmdLCD(GOTO_LINE2_POS0);
	CharLCD((DOM/10) + '0');
	CharLCD((DOM%10) + '0');
	CharLCD('/');
	CharLCD((MONTH/10) + '0');
	CharLCD((MONTH%10) + '0');
	CharLCD('/');
	CharLCD((YEAR/1000) + '0');
	CharLCD((YEAR/100)%10 + '0');
	CharLCD((YEAR%100)/10 + '0');
	CharLCD((YEAR%10) + '0');
}


// Edit functions for time and date
// RTC time edit
void edit_time(void)
{
	// Function to edit time
	// Inputs are taken from KPM and displayed in LCD
	uint8 key;
	
	delay_ms(250);
REAP:	CmdLCD(CLEAR_LCD);
	
	// display options to edit
	StrLCD("1.HOUR 2.MIN");
	CmdLCD(GOTO_LINE2_POS0);
	StrLCD("3.SEC");
	
	CmdLCD(DSP_ON_CUR_NOBLINK);
	CmdLCD(GOTO_LINE2_POS0+8);
	
	// Key scan the option and perform necessary operation
	key = keyScan();
	CharLCD(key);
	delay_ms(250);
	CmdLCD(CLEAR_LCD);
	CmdLCD(GOTO_LINE1_POS0);
	StrLCD("Enter ");
	switch(key)
	{
		case '1':
			// hour
			StrLCD("HOUR: ");
			CmdLCD(GOTO_LINE2_POS0);
			HOUR = U32KeyScan();
			break;
		case '2':
			// min
			StrLCD("MIN: ");
			CmdLCD(GOTO_LINE2_POS0);
			MIN = U32KeyScan();
			break;
		case '3':
			// sec
			StrLCD("SEC: ");
			CmdLCD(GOTO_LINE2_POS0);
			SEC = U32KeyScan();
			break;
		
		default: // repeat
			delay_ms(300);
			CmdLCD(SHIFT_CUR_LEFT);
			CharLCD(' ');
			CmdLCD(SHIFT_CUR_LEFT);
			CmdLCD(DSP_ON_CUR_OFF);
			goto REAP;
		
	}
}

// RTC date edit
void edit_date(void)
{
	// Function to edit date
	// Inputs are taken from KPM and displayed in LCD
	uint8 key;
	delay_ms(250);
REAP: CmdLCD(CLEAR_LCD);
	
	// display options to edit
	StrLCD("1.DATE 2.MONTH");
	CmdLCD(GOTO_LINE2_POS0);
	StrLCD("3.YEAR");
	
	CmdLCD(DSP_ON_CUR_NOBLINK);
	CmdLCD(GOTO_LINE2_POS0+8);
	
	// Key scan the option and perform necessary operation
	key = keyScan();
	CharLCD(key);
	delay_ms(150);
	CmdLCD(CLEAR_LCD);
	CmdLCD(DSP_ON_CUR_OFF);
	switch(key)
	{
		case '1':
			// hour
			CmdLCD(GOTO_LINE1_POS0);
			StrLCD("Enter DATE: ");
			CmdLCD(DSP_ON_CUR_NOBLINK);
			CmdLCD(GOTO_LINE2_POS0);
			DOM = U32KeyScan();
			break;
		case '2':
			// min
			CmdLCD(GOTO_LINE1_POS0);	
			StrLCD("Enter MONTH: ");
			CmdLCD(DSP_ON_CUR_NOBLINK);
			CmdLCD(GOTO_LINE2_POS0);
			MONTH = U32KeyScan();
			break;
		case '3':
			// sec
			CmdLCD(GOTO_LINE1_POS0);
			StrLCD("Enter YEAR: ");
			CmdLCD(DSP_ON_CUR_NOBLINK);
			CmdLCD(GOTO_LINE2_POS0);
			YEAR = U32KeyScan();
			break;
		
		default: // repeat
			delay_ms(300);
			CmdLCD(SHIFT_CUR_LEFT);
			CharLCD(' ');
			CmdLCD(SHIFT_CUR_LEFT);
			goto REAP;
		
	}
}

// edit_date_or_time function
void edit_date_or_time(void)
{
	// Extra function to decide whether to edit date or time
	uint8 key;
	
AFT_PASS: CmdLCD(DSP_ON_CUR_OFF);
	CmdLCD(CLEAR_LCD);
	StrLCD("1. Edit DATE");
	CmdLCD(GOTO_LINE2_POS0);
	StrLCD("2. Edit TIME");
	
	// Enter the valid options
	key = keyScan();
	delay_ms(150);
	
	switch(key)
	{
		case '1': // edit DATE
			edit_date();
			break;
		case '2': // edit TIME
			edit_time();
			break;
		default: // other than '1' or '2' key is pressed, take the input once again
			CmdLCD(CLEAR_LCD);
			CmdLCD(DSP_ON_CUR_OFF);
			StrLCD("Wrong key pressed!");
			CmdLCD(GOTO_LINE2_POS0);
			StrLCD("Press 1 or 2");
			delay_ms(500);
			goto AFT_PASS;
	}
	
}

