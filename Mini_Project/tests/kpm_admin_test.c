// kpm_admin_test.c

#include "../MCU_settings.h"
#include "../mainLPCheader.h"
#include "../types.h"
#include "../delay.h"
#include "../defines.h"
#include "../cfgPortPin.h"
#include "../lcd.h"
#include "../rtc.h"
#include "../kpm.h"
#include "../kpm_admin.h"

#define VIC_ENABLE_EDIT (1<<5)

#define PIN_FUNC4 3
#define EINT0_PIN 1 // @P0.1
#define EINT0_PINFUNC PIN_FUNC4
#define EINT0_VIC_CHNO 14 // EINT1=15, EINT2=16, EINT3=17 in VIC
#define EINT0_STATUS_LED 16 // @P1.16

#define EINT0_EXT_BIT 0

uint32 date=0, month=0, year=0;
uint32 hour=0, min=0, sec=0;

uint32 getInKPM(void)
{
	int32 i=0;
	uint32 res=0;
	uint8 key;
	
	while (1)
	{
		key = keyScan();
		if (i>4)
			break;
		
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
			case 'B': // backspace
					//keyarr[i--] = '\0';
					res = res/10;
					// clear part of cursor
					CmdLCD(SHIFT_CUR_LEFT);
					CharLCD(' ');
					CmdLCD(SHIFT_CUR_LEFT);
						break;
				
		}
		delay_ms(250);
	}
	/*for (i=0; i<5; i++)
	{
		res = (res*10) + (keyarr[i]-'0');
	}*/
	return res;
}

// RTC time edit
void edit_time(void)
{
	uint8 key;
	delay_ms(250);
	CmdLCD(CLEAR_LCD);
	StrLCD("1.HOUR 2.MIN");
	CmdLCD(GOTO_LINE2_POS0);
	StrLCD("3.SEC");
	
	CmdLCD(DSP_ON_CUR_NOBLINK);
	CmdLCD(GOTO_LINE2_POS0+8);
	
	
REAP:	key = keyScan();
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
			HOUR = getInKPM();
			break;
		case '2':
			// min
			StrLCD("MIN: ");
			CmdLCD(GOTO_LINE2_POS0);
			MIN = getInKPM();
			break;
		case '3':
			// sec
			StrLCD("SEC: ");
			CmdLCD(GOTO_LINE2_POS0);
			SEC = getInKPM();
			break;
		
		default: // repeat
			CmdLCD(SHIFT_CUR_LEFT);
			CharLCD(' ');
			CmdLCD(SHIFT_CUR_LEFT);
			goto REAP;
		
	}
}

// ISR declaration
void eint0_isr(void) __irq
{
	uint8 key;
	// ISR Activity Starts
	CmdLCD(CLEAR_LCD);
	CmdLCD(DSP_ON_CUR_NOBLINK);
	StrLCD("ADMIN MODE");
	delay_ms(180);
	admin_enter_password();
	delay_ms(250);
	CmdLCD(DSP_ON_CUR_OFF);
	
AFT_PASS: CmdLCD(CLEAR_LCD);
	StrLCD("1. Edit TIME");
	CmdLCD(GOTO_LINE2_POS0);
	StrLCD("2. Edit MSG");
	
	key = keyScan();
	delay_ms(150);
	switch(key)
	{
		case '1': // RTC time edit
			edit_time();
			break;
		case '2': // MSG enable edit
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
	CmdLCD(DSP_ON_CUR_OFF);
	
	// ISR Activity complete
	EXTINT = 1<<0;
	VICVectAddr = 0;
	
}

int main()
{
	// Cfg P0.1 as EINT0 pin
	cfgPortPin(0, EINT0_PIN, EINT0_PINFUNC);
	
	// Cfg VIC block
	
	// Enable the VIC block
	VICIntEnable = (1<<EINT0_VIC_CHNO);
	
	// Select the EINT0 interrupt as IRQ interrupt
	VICIntSelect = 0; // selected all interrupts as irq type
	
	// Enable ISR edit option on a particular channel assigned to EINT0 in VIC table (EINT0 channel no. is 14)
	VICVectCntl0 = VIC_ENABLE_EDIT | EINT0_VIC_CHNO;
	
	// assign ISR address to VIC
	VICVectAddr0 = (uint32) eint0_isr;
	
	// As the interrupt is External Interrupt, Configure the External Interrupt Block
	
	// Enable the External Interrupt
	EXTINT |= (1<<EINT0_EXT_BIT);
	
	// Select the type of triggering the interrupt as Edge triggering
	EXTMODE |= (1<<EINT0_EXT_BIT); // '1' -> Edge triggering, '0' -> Level Triggering
	
	// Select the triggering transition, Rising Edge(1) or Falling Edge (0)
	// Choosing Falling Edge (0) by-default
	EXTPOLAR &= ~(1<<EINT0_EXT_BIT);

	InitRTC();
	InitKPM();
	InitLCD();
	CmdLCD(DSP_ON_CUR_OFF);
	StrLCD("  USER MODE  ");
	delay_ms(250);
	CmdLCD(CLEAR_LCD);
	while(1)
	{
		
		getDate(&date, &month, &year);
		getTime(&hour, &min, &sec);
		
		// Show time
		CmdLCD(GOTO_LINE1_POS0);
		CharLCD((hour/10) + '0');
		CharLCD((hour%10) + '0');
		CharLCD(':');
		CharLCD((min/10) + '0');
		CharLCD((min%10) + '0');
		CharLCD(':');
		CharLCD((sec/10) + '0');
		CharLCD((sec%10) + '0');
		
		CmdLCD(GOTO_LINE2_POS0);
		CharLCD((date/10) + '0');
		CharLCD((date%10) + '0');
		CharLCD('/');
		CharLCD((month/10) + '0');
		CharLCD((month%10) + '0');
		CharLCD('/');
		CharLCD((year/10)/10 + '0');
		CharLCD((year/10)%10 + '0');
		CharLCD((year%10)/10 + '0');
		CharLCD((year%10) + '0');
	}
}
