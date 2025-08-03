// ADC.c

#include "MCU_settings.h"
#include "mainLPCheader.h"
#include "types.h"
#include "delay.h"
#include "defines.h"
#include "cfgPortPin.h"
#include "ADC_defines.h"

#define val(x) x
#define token(x,y) x##y


#ifdef __LPC2129

void InitADC(uint8 pinNo, uint8 pinFunc)
{
	// ADC Initialization for LPC2129
	
	// Cfg ADC pinNo as ADC input function
	cfgPortPin(0, pinNo, pinFunc);
	// Cfg ADCR SFR
	ADCR = ((1<<PDN_BIT) | (CLKDIV<<CLKDIV_BIT));
}

void ReadADC(uint8 chNo, real32 *eAR, uint32 *dVal)
{
	// Function to read value from ADC for LPC2129
	
	
	// clear prev channel selection
	WRITEBYTE(ADCR, 0, ~(0xFF));
	// Select analog channel, start conversion
	// check for BURST_MODE
	#if BURST_MODE == 1
		ADCR |= (1<<BURST_BIT) | (0<<START_BIT) | (1<<chNo);
	#else
		ADCR |= (1<<START_BIT) | (1<<chNo);
	#endif
	// conversion time 2.44us
	delay_us(3);
	// check for done bit
	while(READBIT(ADDR, DONE_BIT)==1);
	
	// if conversion is completed, stop ADC if BURST_MODE == 0
	#if BURST_MODE == 0
		// Stop conversion
		ADCR &= ~(1<<START_BIT);	
	#endif
	
	// read 10-bit digital data
	*dVal = ((ADDR>>RESULT_BIT)&1023);
	*eAR = (*dVal * STEP_SIZE);
}

#elif defined(__LPC2148)

void InitADC(uint8 pinNo, uint8 adcPort, uint8 pinFunc)
{
	// ADC Initialization for LPC2148 
	
	// Cfg ADC pinNo as ADC input function
	cfgPortPin(0, pinNo, pinFunc);
	// Cfg ADCR SFR
	if (adcPort == 0)
		AD0CR = ((1<<PDN_BIT) | (CLKDIV<<CLKDIV_BIT));
	else if (adcPort == 1)
		AD1CR = ((1<<PDN_BIT) | (CLKDIV<<CLKDIV_BIT));
}

void ReadADC(uint8 chNo, uint8 adcPort, real32 *eAR, uint32 *dVal)
{
	// Function to read value from ADC for LPC2148
	
	// clear prev channel selection
	//WRITEBYTE(token(token(AD, val(adcPort)), CR), 0, ~(0xFF));
	if (adcPort == 0)
		AD0CR = (AD0CR & ~(0xFF<<0));
	else if (adcPort == 1)
		AD1CR = (AD1CR & ~(0xFF<<0));
	// Select analog channel, start conversion
	// check for BURST_MODE
	#if BURST_MODE == 1
		if (adcPort == 0)
			AD0CR |= (1<<BURST_BIT) | (0<<START_BIT) | (1<<chNo);
		else if (adcPort == 1)
			AD1CR |= (1<<BURST_BIT) | (0<<START_BIT) | (1<<chNo);
	#else
		if (adcPort == 0)
			AD0CR |= (1<<START_BIT) | (1<<chNo);
		else if (adcPort == 1)
			AD1CR |= (1<<START_BIT) | (1<<chNo);
	#endif
		
	// conversion time 2.44us
	delay_us(3);
	// check for done bit
	if (adcPort == 0)
		while (((AD0GDR>>DONE_BIT)&1) == 0);
	else if (adcPort == 1)
		while (((AD1GDR>>DONE_BIT)&1) == 0);
	
	// if conversion is completed, stop ADC if BURST_MODE == 0
	#if BURST_MODE == 0
		// Stop conversion
		if (adcPort == 0)
			AD0CR &= ~(1<<START_BIT);
		else if (adcPort == 1)
			AD1CR &= ~(1<<START_BIT);
	#endif
	
	// read 10-bit digital data
	if (adcPort == 0)
		*dVal = ((AD0GDR >> RESULT_BIT)&1023);
	else if (adcPort == 1)
		*dVal = ((AD1GDR >> RESULT_BIT)&1023);

	*eAR = (*dVal * STEP_SIZE);
}

#endif
