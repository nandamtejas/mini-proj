// rtc.h

#include "types.h"

#ifndef _RTC_H
#define _RTC_H

#include "rtc_defines.h"

void InitRTC(void);

// Date and time
void setTime(uint32 hour, uint32 min, uint32 sec);
void getTime(uint32 *hour, uint32 *min, uint32 *sec);
void setDate(uint32 date, uint32 month, uint32 year);
void getDate(uint32 *date, uint32 *month, uint32 *year);

// Alarm
void setAlarmTime(uint32 hour, uint32 min);

// display Clock
void displayClock(void);

// edit functions
void edit_time(void);
void edit_date(void);
void edit_date_or_time(void);

#endif
