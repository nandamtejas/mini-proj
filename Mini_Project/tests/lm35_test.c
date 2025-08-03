// lm35_test.c
#include "../MCU_settings.h"
#include "../mainLPCheader.h"
#include "../types.h"
#include "../delay.h"
#include "../lcd_defines.h"
#include "../lcd.h"
#include "../lm35_defines.h"
#include "../lm35.h"

int main()
{
	real32 temp=0.0;
	InitLCD();
	CmdLCD(DSP_ON_CUR_OFF);
	StrLCD("LM35 TEST\n");
	delay_ms(150);
	while (1)
	{
		CmdLCD(GOTO_LINE2_POS0);
		StrLCD("Temp: ");
		ReadLM35(&temp);
		F32LCD(temp, 2);
		delay_ms(50);
	}
}
