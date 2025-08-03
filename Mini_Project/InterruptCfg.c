// InterruptCfg.c

#include "MCU_settings.h"
#include "mainLPCheader.h"
#include "types.h"
#include "delay.h"
#include "defines.h"
#include "cfgPortPin.h"
#include "ISR.h"
#include "InterruptCfg_defines.h"

void InitInterrupt(void)
{
	// This function handles Interrupt Initialization
	
	// Cfg P0.1 as EINT0 pin
	cfgPortPin(0, EINT0_PIN, EINT0_PINFUNC);
	
	// Cfg VIC block
	
	// Enable the VIC block for EINT0 and RTC channels
	VICIntEnable = (1<<EINT0_VIC_CHNO) | (1<<ALARM_VIC_CHNO);;
	
	// Select the EINT0 interrupt as FIQ interrupt, Alarm interrupt as IRQ
	VICIntSelect = (1<<EINT0_VIC_CHNO); // selected all interrupts as irq type
	
	// Enable ISR edit option on a particular channel assigned to Alarm in VIC table (EINT0 channel no. is 14)
	VICVectCntl0 = (VIC_ENABLE_EDIT | ALARM_VIC_CHNO);
	
	// assign rtc_alarm_isr ISR address to VIC (vector IRQ)
	VICVectAddr0 = (uint32) rtc_alarm_isr;
	
	// As the interrupt is External Interrupt, Configure the External Interrupt Block
	
	
	/* Enable the External Interrupt */
	EXTINT |= (1<<EINT0_EXT_BIT);
	
	// Select the type of triggering the interrupt as Edge triggering
	EXTMODE |= (1<<EINT0_EXT_BIT); // '1' -> Edge triggering, '0' -> Level Triggering
	
	// Select the triggering transition, Rising Edge(1) or Falling Edge (0)
	// Choosing Falling Edge (0) by-default
	EXTPOLAR &= ~(1<<EINT0_EXT_BIT);
	
	/* RTC ALARM Interrupt */
	WRITEBIT(ILR, RTCALF_BIT, 1);
	WRITEBYTE(AMR, 0, ~(0x6));
}
