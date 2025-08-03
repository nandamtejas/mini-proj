// rtc_test.c

#include "../MCU_settings.h"
#include "../mainLPCheader.h"
#include "../types.h"
#include "../delay.h"
#include "../lcd.h"
#include "../rtc.h"

uint32 date=22, month=7, year=2022;
uint32 hour=20, min=30, sec=0;

void displayTime()
{
	CharLCD((hour/10)+'0');
	CharLCD((hour%10)+'0');
	CharLCD(':');
	CharLCD((min/10)+'0');
	CharLCD((min%10)+'0');
	CharLCD(':');
	CharLCD((sec/10)+'0');
	CharLCD((sec%10)+'0');
}

void displayDate()
{
	CharLCD((date/10)+'0');
	CharLCD((date%10)+'0');
	CharLCD('/');
	CharLCD((month/10)+'0');
	CharLCD((month%10)+'0');
	CharLCD('/');
	CharLCD(((year/100)/10)+'0');
	CharLCD(((year/100)%10)+'0');
	CharLCD(((year%100)/10)+'0');
	CharLCD(((year%100)%10)+'0');
}

int main()
{
	// RTC test, display the time in LCD
	InitLCD();
	CmdLCD(DSP_ON_CUR_OFF);
	StrLCD("RTC TEST");
	delay_ms(250);
	
	
	// RTC init
	InitRTC();
	
	#if (CLKSRC == 0) // set to PREScalar
		setTime(hour, min, sec);
		setDate(date, month, year);
	#endif
	
	while(1)
	{
		getDate(&date, &month, &year);
		getTime(&hour, &min, &sec);
		
		CmdLCD(GOTO_LINE1_POS0+4);
		displayDate();
		CmdLCD(GOTO_LINE2_POS0+4);
		displayTime();
		//delay_ms(250);
	}
}
