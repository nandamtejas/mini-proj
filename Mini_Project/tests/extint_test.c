// extint_test.c

#include "../MCU_settings.h"
#include "../mainLPCheader.h"
#include "../types.h"
#include "../delay.h"
#include "../cfgPortPin.h"
#include "../lcd.h"

#define VICEnableEditISR (1<<5)

#define PIN_FUNC4 3
#define EINT0_PIN 1 // @P0.1
#define EINT0_PINFUNC PIN_FUNC4
#define EINT0_VIC_CHNO 14 // EINT1=15, EINT2=16, EINT3=17 in VIC
#define EINT0_STATUS_LED 16 // @P1.16


// ISR
void eint0_isr(void) __irq;

int32 gCount=0;
int main()
{
	// Cfg P0.1 as External Interrupt pin
	cfgPortPin(0, EINT0_PIN, EINT0_PINFUNC);
	
	// Initialize VIC
	// Select Interrupt type as IRQ ['1' as FIQ; '0' as IRQ] [default: 0(IRQ)]
	VICIntSelect = (VICIntSelect &~(1<<EINT0_VIC_CHNO));
	
	// Enable interrupt
	VICIntEnable = (1<<EINT0_VIC_CHNO);
	
	// Cfg EINT0 as highest priority (0) by writing channel number into VICVectCntl0 SFR along with enabling edit
	VICVectCntl0 = VICEnableEditISR | EINT0_VIC_CHNO; 
	
	// Write the address of ISR into VICVectAddr0
	VICVectAddr0 = (uint32) eint0_isr;
	
	// Now Initialize the External Interrupt Block
	// Enable which interrupt should be read (here EINT0)
	EXTINT = (1<<0);
	// Enable the mode of the interrupe [level-triggering(0) or Edge-triggering(1)]
	EXTMODE = (1<<0);
	// Select rising edge(1) or falling edge(0)
	EXTPOLAR = (1<<0); // rising edge
	
	// Initialize LCD
	InitLCD();
	CmdLCD(DSP_ON_CUR_OFF);
	CmdLCD(GOTO_LINE1_POS0);
	StrLCD("Interrupt Test");
	delay_ms(750);
	
	// Cfg LED pin as gpio out pin
	IODIR1 = ((IODIR1 & ~(1<<EINT0_STATUS_LED)) | (1<<EINT0_STATUS_LED)); 
	
	// main program
	while(1)
	{
		CmdLCD(GOTO_LINE1_POS0);
		StrLCD("Main Program    ");
		CmdLCD(GOTO_LINE2_POS0);
		StrLCD("gCount: ");
		U32LCD(gCount++);
		delay_ms(250);
	}
}

void eint0_isr() __irq
{
	// ISR activity starts
	
	// Clear LCD
	CmdLCD(CLEAR_LCD);
	CmdLCD(GOTO_LINE1_POS0);
	StrLCD("Interrupt Occur ");
	CmdLCD(GOTO_LINE2_POS0);
	StrLCD("LED Toggles");
	delay_ms(250);
	// Toggle the LED
	IOPIN1 |= (1<<EINT0_STATUS_LED);
	delay_ms(250);
	IOPIN1 &= ~(1<<EINT0_STATUS_LED);
	delay_ms(250);
	
	CmdLCD(CLEAR_LCD);
	StrLCD("Byee!!");
	delay_ms(250);
	CmdLCD(CLEAR_LCD);
	// isr activity ends
	// clear the EXTINT and VICVectAddr
	EXTINT = 1<<0;
	VICVectAddr = 0;
	
}
