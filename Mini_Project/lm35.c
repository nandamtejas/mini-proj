// lm35.c
#include "MCU_settings.h"
#include "mainLPCheader.h"
#include "types.h"
#include "delay.h"
#include "lcd.h"
#include "ADC_defines.h"
#include "ADC.h"
#include "lm35_defines.h"

// Temperature sensor global variables
real32 temp=0.0;

void ReadLM35(real32 *temp)
{
	// Function to read temperature from temperature Sensor through ADC channel
	
	uint32 dVal=0;
	static uint32 flag=0;
	
	if (flag == 0)
	{
		// If the ADC is not Initialized, Initialize the ADC
		#ifdef __LPC2129
		InitADC(ADC_IN_PIN, ADC_IN_PINFUNC);
		#elif defined(__LPC2148)
			InitADC(ADC_IN_PIN, ADC_IN_PORT, ADC_IN_PINFUNC);
		#endif
		flag=1;
	}
	
	// ReadADC
	#ifdef __LPC2129
		ReadADC(ADC_IN_CH, &eAR, &dVal);
	#elif defined(__LPC2148)
		ReadADC(ADC_IN_CH, ADC_IN_PORT, temp, &dVal);
	#endif
	
	// temperature calculations
	*temp = *temp*100;
}

void displayTemp(void)
{
	// Function to display temperature
	
	// get the current temperature from ADC
	ReadLM35(&temp);
	
	// Show Temperature
	CmdLCD(SHIFT_CUR_RIGHT);
	F32LCD(temp, 1);
	//StrLCD("oC");
	CharLCD(0);
}
