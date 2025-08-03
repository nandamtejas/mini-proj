// lm35_defines.h

#ifndef _LM35_DEFINES_H
#define _LM35_DEFINES_H

#include "pinfunc.h"

#define ADC_BURST 0

#ifdef __LPC2129
	#define ADC_IN_CH 1
	#define ADC_IN_PIN 28
	#define ADC_IN_PINFUNC PINFUNC2
#elif defined(__LPC2148)
	#define ADC_IN_CH 3
	#define ADC_IN_PORT 0
	#define ADC_IN_PIN 30
	#define ADC_IN_PINFUNC PINFUNC1
#endif

#endif
