// adc_test.c

#include "../MCU_settings.h"
#include "../mainLPCheader.h"
#include "../types.h"
#include "../delay.h"
#include "../defines.h"
#include "../cfgPortPin.h"
#include "../lcd_defines.h"
#include "../lcd.h"
#include "../ADC_defines.h"
#include "../ADC.h"

#ifdef __LPC2129
	#define ADC_IN_CH 1
	#define ADC_IN_PIN 28
	#define ADC_IN_PINFUNC PINFUNC2
#elif defined(__LPC2148)
	#define ADC_IN_CH 3
	#define ADC_IN_PORT 0
	#define ADC_IN_PIN AD0CH3
	#define ADC_IN_PINFUNC PINFUNC1
#endif

main()
{
	uint32 dVal=0;
	real32 eAR=0.0;
	#ifdef __LPC2129
		InitADC(ADC_IN_PIN, ADC_IN_PINFUNC);
	#elif defined(__LPC2148)
		InitADC(ADC_IN_PIN, ADC_IN_PORT, ADC_IN_PINFUNC);
	#endif
	InitLCD();
	CmdLCD(DSP_ON_CUR_OFF);
	StrLCD("ADC_test");
	delay_ms(500);
	
	while(1)
	{
		CmdLCD(GOTO_LINE2_POS0);
		StrLCD("Temp: ");
		#ifdef __LPC2129
			ReadADC(ADC_IN_CH, &eAR, &dVal);
		#elif defined(__LPC2148)
			ReadADC(ADC_IN_CH, ADC_IN_PORT, &eAR, &dVal);
		#endif
		F32LCD(eAR*100, 2);
		StrLCD("oC");
	}
	
}
