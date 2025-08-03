// ISR.h

#ifndef _ISR_H
#define _ISR_H

// ISR declaration for EINT0
void eint0_isr(void) __irq;

// ISR declaration for ALARM Interrupt
void rtc_alarm_isr(void) __irq;

#endif
