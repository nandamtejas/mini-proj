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


// Defines for Mode Status LED
#define CLOCK_MODE_STATUS_LED 25 // @P1.25
#define MESSAGE_SCROLL_MODE_STATUS_LED 26 // @P1.26
#define ADMIN_MODE_STATUS_LED 27 // @P1.27

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
#define ISINRANGE(val, x, y) ISWITHIN(val, x, (y-1))
#define ISNOTINRANGE(val, x, y) ISNOTWITHIN(val, x, (y-1))
#define ISLEAPYEAR(val) ( ( ((val%4)==0) && ((val%100)!=0) ) || ((val%400)==0) ) 
#define IS30DAYMONTH(month) (((month)==4)||((month)==6)||((month)==9)||((month)==11))
#define IS31DAYMONTH(month) (!(((month)==2) || IS30DAYMONTH(month)))
#define MAXIMUM(x,y) ( ((x)>(y)) ? (x):(y) )
#define MINIMUM(x,y) ( ((x)<(y)) ? (x):(y) )
#endif
