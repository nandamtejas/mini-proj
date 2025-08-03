// MCU_settings.h

#ifndef _MCU_SETTINGS_H
#define _MCU_SETTINGS_H

// MCU 
#define __LPC2148

// ADC SETTINGS
#define BURST_MODE ADC_BURST

// RTC SETTINGS
#define RTC_CLKSRC CLKSRC 
#define RTC_ALARM_EN ALARM_EN

// ADMIN SETTINGS
#define ADMIN_PASSWORD "0123"

//#define NULL ((void *) 0)

// Simple MACRO Definitions
#define ISEQUAL(x,y) ((x)==(y))
#define ISGREATER(x,y) ((x)>(y))
#define ISLESSER(x,y) ((x)<(y))
#define ISGTEQ(x,y) ((x)>=(y))
#define ISLTEQ(x,y) ((x)<=(y))
#define ISWITHIN(val, x, y) (x<y) ? (((val)>=(x)) && ((val)<=(y))) : (((val)>=(y)) && ((val)<=(x)))
#define ISNOTWITHIN(val, x, y) (!(ISWITHIN(val, x, y)))
#define ISBETWEEN(val, x, y) (x<y) ? (((val)>(x)) && ((val)<(y))) : (((val)>(y)) && ((val)<(x)))
#define ISNOTBETWEEN(val, x, y) (!(ISNOTBETWEEN(val, x, y)))

#endif
