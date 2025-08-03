// message_marquee_test.c

#include "../MCU_settings.h"
#include "../mainLPCheader.h"

#include "../types.h"
#include "../delay.h"
#include "../defines.h"
#include "../message_marquee.h"
#include "../lcd_defines.h"
#include "../lcd.h"

// test message
uint8 msg[64] = "This is very long message";
// As we are using 16x2 LCD to display where LCD has 2 lines and each line can hold 16 characters each, hence the WINDOW size if 16
#define WINDOW 16

int main()
{
	
	InitLCD();
	CmdLCD(DSP_ON_CUR_OFF);
	// Initialize LCD
	// Test LCD
	StrLCD("Message Marquee Test");
	CmdLCD(CLEAR_LCD);
	delay_ms(100);
	
	while(1)
	{
		message_marquee(msg);
	}
}
