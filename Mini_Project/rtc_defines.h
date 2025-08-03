// rtc_defines.h

#ifndef _RTC_DEFINES_H
#define _RTC_DEFINES_H

#define FOSC (12*1e6)
#define CCLK (FOSC*5) // 60MHz
#define PCLK (CCLK/4) // 15MHz

// Prefactor calculations
#define PREINT_FACT (int)((PCLK/32768)-1)
#define PREFRAC_FACT (PCLK - ((PREINT_FACT+1) * 32768))

// defines for CCR 
#define CLKEN_BIT 0
#define CLKSRC_BIT 4
#define RTC_ENABLE (1<<CLKEN_BIT)
#define RTC_SRC (1<<CLKSRC_BIT)
#define RTC_RESET 0x00

// defines for ILR
#define RTCCIF_BIT 0
#define RTCALF_BIT 1

// RTC global settings
// define CLKSRC 0-pre-scaler 1-RTC oscillator
#define CLKSRC 1
// define ALARM_EN 1-Alarm interrupt enabled; 0-Alarm interrupt disabled
#define ALARM_EN 1

#endif
