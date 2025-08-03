// ADC.h

#include "types.h"

#ifndef _ADC_H
#define _ADC_H
#include "ADC_defines.h"

#ifdef __LPC2129
	void InitADC(uint8 chNo, uint8 pinFunc);
	void ReadADC(uint8 chNo, real32 *eAR, uint32 *dVal);
#elif defined(__LPC2148)
	void InitADC(uint8 chNo, uint8 adcPort, uint8 pinFunc);
	void ReadADC(uint8 chNo, uint8 adcPort, real32 *eAR, uint32 *dVal);
#endif

#endif
