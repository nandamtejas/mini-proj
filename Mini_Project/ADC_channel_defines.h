// ADC_CHN_defines.h

#ifndef _ADC_CHN_DEFINES_H
#define _ADC_CHN_DEFINES_H

// If selected MCU is LPC2129
#ifdef __LPC2129
	// ADC Channels
	#define ADCH0 27 // P0.27 // PINFUNC1
	#define ADCH1 28 // P0.28 // PINFUNC1
	#define ADCH2 29 // P0.29 // PINFUNC1
	#define ADCH3 30 // P0.30 // PINFUNC1
    
#elif defined(__LPC2148)
    // ADC0 Channels
	#define AD0CH0 27 // P0.27 // not in LPC2148 // PINFUNC1
	#define AD0CH1 28 // P0.28 // PINFUNC1
	#define AD0CH2 29 // P0.29 // PINFUNC1
	#define AD0CH3 30 // P0.30 // PINFUNC1
	#define AD0CH4 25 // P0.25 // PINFUNC2
	#define AD0CH6 4 // P0.4 // PINFUNC4
	#define AD0CH7 5 // P0.5 // PINFUNC4

	// ADC1 Channels
	#define AD1CH0 6 // P0.6 // PINFUNC4
	#define AD1CH1 8 // P0.8 // PINFUNC4
	#define AD1CH2 10 // P0.10 // PINFUNC4
	#define AD1CH3 12 // P0.12 // PINFUNC4
	#define AD1CH4 13 // P0.13 // PINFUNC4
	#define AD1CH5 15 // P0.15 // PINFUNC4
	#define AD1CH6 21 // P0.21 // PINFUNC3
	#define AD1CH7 22 // P0.22 // PINFUNC2
	
#endif

#endif
