// ADC_defines.h

#ifndef _ADC_DEFINES_H
#define _ADC_DEFINES_H

#include "ADC_channel_defines.h"

// ADC operating frequency defines
#define FOSC (12*1e6)
#define CCLK (FOSC*5)
#define PCLK (CCLK/4)
#define ADCCLK (3*1e6)
#define CLKDIV (int32)((PCLK/ADCCLK)-1)
#define VREF 3.3 // Vref = 3.3V
#define RESOLUTION 10 // 10bit ADC resolution
#define STEP_SIZE (VREF/(1<<RESOLUTION))

// AD0CR/AD1CR SFR defines
#define CLKDIV_BIT 8 // AD0CR@bit8-bit15 (8bits)
#define PDN_BIT 21 // PDN Bit always set to 1 to turn ON ADC 
#define BURST_BIT 16 // BURST_BIT (0 for one-time conversion, 1 for continuous conversion)
#define START_BIT 24 // 3-bit START bit to start ADC conversion, if BURST bit is 1 -> START_BIT = 000
#define EDGE_BIT 27 // EDGE bit is only for when START bit is from 010-111 (1 indicates falling edge; 0 indicates raising edge) used in interrupt mode

// AD0GDR/AD1GDR SFR
#define RESULT_BIT 6 // AD0GDR or AD0DR(0-6) has same bits
#define DONE_BIT 31 // AD0GDR done bit, (if conversion is over, DONE_BIT is set to 1)


// ADC global settings
#define ADC_BURST 0 

#endif
