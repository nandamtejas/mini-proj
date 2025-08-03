// InterruptCfg_defines.h


#ifndef _INTERRUPT_DEFINES_H
#define _INTERRUPT_DEFINES_H

// VIC defines
#define VIC_ENABLE_EDIT (1<<5)

// External interrupt defines
#define PIN_FUNC4 3
#define EINT0_PIN 1 // @P0.1
#define EINT0_PINFUNC PIN_FUNC4
#define EINT0_VIC_CHNO 14 // EINT1=15, EINT2=16, EINT3=17 in VIC
#define EINT0_STATUS_LED 16 // @P1.16

#define EINT0_EXT_BIT 0

// Alarm interrupt defines
#define ALARM_VIC_CHNO 13 // For RTC the Channel number in VIC table is 13
#define RTCALF_BIT 1

#endif
