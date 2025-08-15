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
	ALHOUR = hour;
	ALMIN = min;
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
}


// Edit functions for time and date
// RTC time edit
void edit_time(void)
{
	// Function to edit time
	// Inputs are taken from KPM and displayed in LCD
	uint8 key;
	
	delay_ms(150);
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
	switch(key)
	{
		case '1':
			// hour
REAP_H:		StrLCD("Enter HOUR");
			CmdLCD(GOTO_LINE2_POS0);
			StrLCD("(1-23): ");
			HOUR = U32KeyScan();
			// if hour is not in range (0-23)
			if (ISNOTWITHIN(HOUR, 1, 23))
			{
				// if not in range, repeat
				delay_ms(50);
				CmdLCD(DSP_ON_CUR_OFF);
				CmdLCD(CLEAR_LCD);
				StrLCD("HOUR not in");
				CmdLCD(GOTO_LINE2_POS0);
				StrLCD("range (1-23)");
				delay_ms(250);
				CmdLCD(CLEAR_LCD);
				goto REAP_H;
			}
			break;
		case '2':
			// min
REAP_M:		StrLCD("Enter MIN");
			CmdLCD(GOTO_LINE2_POS0);
			StrLCD("(0-59): ");
			MIN = U32KeyScan();
			// if min is not in range (0-59)
			if (ISNOTWITHIN(((int32)MIN), 0, 59))
			{
				// if not in range, repeat
				delay_ms(50);
				CmdLCD(DSP_ON_CUR_OFF);
				CmdLCD(CLEAR_LCD);
				StrLCD("MIN not in");
				CmdLCD(GOTO_LINE2_POS0);
				StrLCD("range (0-59)");
				delay_ms(250);
				CmdLCD(CLEAR_LCD);
				goto REAP_M;
			}
			break;
		case '3':
			// sec
REAP_S:		StrLCD("Enter SEC");
			CmdLCD(GOTO_LINE2_POS0);
			StrLCD("(0-59): ");
			SEC = U32KeyScan();
			// if min is not in range (0-59)
			if (ISNOTWITHIN(((int32)SEC), 0, 59))
			{
				// if sec not in range, repeat
				delay_ms(50);
				CmdLCD(CLEAR_LCD);
				CmdLCD(DSP_ON_CUR_OFF);
				StrLCD("SEC not in");
				CmdLCD(GOTO_LINE2_POS0);
				StrLCD("range (0-59)");
				delay_ms(250);
				CmdLCD(CLEAR_LCD);
				goto REAP_S;
			}
			break;
		
		default: // repeat
			delay_ms(150);
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
	delay_ms(150);
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
			// date
REAP_D:		CmdLCD(GOTO_LINE1_POS0);
			StrLCD("Enter DATE: ");
			CmdLCD(DSP_ON_CUR_NOBLINK);
			CmdLCD(GOTO_LINE2_POS0);
			DOM = U32KeyScan(); 
			// if FEB 
REAP_D_COND:/*if ((DOM>28) && (MONTH==2))
			{
				delay_ms(50);
				CmdLCD(DSP_ON_CUR_OFF);
				CmdLCD(CLEAR_LCD);
				StrLCD("Date not in");
				CmdLCD(GOTO_LINE2_POS0);
				// If Feb month out of range,
				if (!(ISLEAPYEAR(YEAR)))
				{
					// condition for not a leap year 
					StrLCD("range (1-28)");							
				}
				else if (ISLEAPYEAR(YEAR) && (DOM>29))
				{
					// condition for leap year and date is not 29
					StrLCD("range (1-29)");
				}
				else
					break;
				delay_ms(250);
				CmdLCD(CLEAR_LCD);
				goto REAP_D;
			}*/
			if (MONTH==2)
			{
				if ( ( (!(ISLEAPYEAR(YEAR))) && (DOM>28) ) || (ISLEAPYEAR(YEAR) && (DOM>29)) )
				{
					delay_ms(50);
					CmdLCD(DSP_ON_CUR_OFF);
					CmdLCD(CLEAR_LCD);
					StrLCD("Date not in");
					CmdLCD(GOTO_LINE2_POS0);
				}
				
				if ( (!(ISLEAPYEAR(YEAR))) && (DOM>28) )
				{
					// condition for not a leap year 
					StrLCD("range (1-28)");							
				}
				else if (ISLEAPYEAR(YEAR) && (DOM>29))
				{
					// condition for leap year and date is not 29
					StrLCD("range (1-29)");
				}
				else
					break;
				delay_ms(250);
				CmdLCD(CLEAR_LCD);
				goto REAP_D;
			}
			else if ((DOM>30) && IS30DAYMONTH(MONTH))
			{
				// if months are April, June, September, November and date is greater than 30
				delay_ms(50);
				CmdLCD(DSP_ON_CUR_OFF);
				CmdLCD(CLEAR_LCD);
				StrLCD("Date not in");
				CmdLCD(GOTO_LINE2_POS0);
				StrLCD("range (1-30)");
				delay_ms(250);
				CmdLCD(CLEAR_LCD);
				goto REAP_D;
			}
			else if (DOM>31)
			{
				// No date is greater than 31
				delay_ms(50);
				CmdLCD(DSP_ON_CUR_OFF);
				CmdLCD(CLEAR_LCD);
				StrLCD("Date not in");
				CmdLCD(GOTO_LINE2_POS0);
				StrLCD("range (1-31)");
				delay_ms(250);
				CmdLCD(CLEAR_LCD);
				goto REAP_D;
			}
			break;
		case '2':
			// month
REAP_M:		CmdLCD(GOTO_LINE1_POS0);	
			StrLCD("Enter MONTH");
			CmdLCD(GOTO_LINE2_POS0);
			StrLCD("(1-12): ");
			CmdLCD(DSP_ON_CUR_NOBLINK);
			MONTH = U32KeyScan();
			// if month not in range
			if (ISNOTWITHIN(MONTH, 1, 12))
			{
				// if month not in range, repeat
				delay_ms(50);
				CmdLCD(DSP_ON_CUR_OFF);
				CmdLCD(CLEAR_LCD);
				StrLCD("MONTH not in");
				CmdLCD(GOTO_LINE2_POS0);
				StrLCD("range (1-12)");
				delay_ms(250);
				CmdLCD(CLEAR_LCD);
				goto REAP_M;
			}
			// There is a complication, 
			// Depending on month, the date ranges changes
			// If month is modified and corresponding date is not in the range,
			// Admin has to enter the new date forcefully
			if ((MONTH == 2))
			{
				// if month is 2, check for leapyear and date
				if ( ( (!(ISLEAPYEAR(YEAR))) && (DOM>28) ) || (ISLEAPYEAR(YEAR) && (DOM>29)) )
				{
					// if year is not leap year and date is more than 28, 
					// or if the year is leap year yet the date is more than 29,
					// Admin should forcefully enter the new date
					goto DATE_MODIFY;
				}
			}
			else if (IS30DAYMONTH(MONTH) && (DOM>30))
			{
				// if month is 30 day month, and the date is more than 30 (lets say 31), 
				// Admin should forcefully enter the new date
				goto DATE_MODIFY;
			}
			else if (IS31DAYMONTH(MONTH) && (DOM>31) )
			{
				// if month is 31 day month, and date is more than 30 days
				// Admin should forcefully enter the new date
				goto DATE_MODIFY;
			}
			break;
			
		case '3':
			// year
REAP_Y:		CmdLCD(GOTO_LINE1_POS0);
			StrLCD("Enter YEAR");
			CmdLCD(GOTO_LINE2_POS0);
			StrLCD("(1900-3000):");
			CmdLCD(DSP_ON_CUR_NOBLINK);
			YEAR = U32KeyScan();
			// if year not in range
			if (ISNOTWITHIN(YEAR, 1900, 3000))
			{
				// if month not in range, repeat
				delay_ms(50);
				CmdLCD(DSP_ON_CUR_OFF);
				CmdLCD(CLEAR_LCD);
				StrLCD("YEAR not in");
				CmdLCD(GOTO_LINE2_POS0);
				StrLCD("range (1900-3000)");
				delay_ms(250);
				CmdLCD(CLEAR_LCD);
				goto REAP_Y;
			}
			// check if the year is changed let's say leap year to non leap year, 
			// Check the February month (month=2) if the date is exceeds the range(1-29)
			if ((!(ISLEAPYEAR(YEAR))) && (MONTH==2) && (DOM>=29))
			{
				// If the current year is not leap year and current month is 2 and date is 29
				// Admin should forcefully enter the new date
				goto DATE_MODIFY;
			}
			break;
		
		default: // repeat
			delay_ms(300);
			CmdLCD(SHIFT_CUR_LEFT);
			CharLCD(' ');
			CmdLCD(SHIFT_CUR_LEFT);
			goto REAP;
		
	}
	return;

// Date Modify condition
DATE_MODIFY:	delay_ms(50);
				CmdLCD(DSP_ON_CUR_OFF);
				CmdLCD(CLEAR_LCD);
				StrLCD("Current Date is");
				CmdLCD(GOTO_LINE2_POS0);
				StrLCD("out of range");
				delay_ms(150);
				CmdLCD(CLEAR_LCD);
				goto REAP_D_COND;
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
			delay_ms(250);
			goto AFT_PASS;
	}
	
}

