// software_interrupt_test.c

#include <LPC214x.h>
#include "../delay.h"

// Define Interrupt Registers
/*
#define VICIntEnable     (*(volatile unsigned long *)0xFFFFF010)
#define VICSoftInt       (*(volatile unsigned long *)0xFFFFF018)
#define VICSoftIntClear  (*(volatile unsigned long *)0xFFFFF01C)
#define VICVectAddr0     (*(volatile unsigned long *)0xFFFFF100)
#define VICVectCntl0     (*(volatile unsigned long *)0xFFFFF200)
#define VICVectAddr      (*(volatile unsigned long *)0xFFFFF030)
*/
#define VICSoftIntClear  (*(volatile unsigned long *)0xFFFFF01C)
// Use Timer0 Interrupt for simulation (IRQ source 4 in LPC2148)
#define TIMER0_IRQ_NUM   4

volatile int interrupt_flag = 0;  // Just to show ISR was called

// Dummy ISR for Timer0 — triggered via software
__irq void timer0_isr(void) {
    interrupt_flag = 1;   // ISR sets a flag
    // Normally you'd clear T0IR here for real timer use

    VICVectAddr = 0;  // Acknowledge ISR to VIC
}

void init_vic(void) {
    // Assign ISR to vector slot 0
    VICVectAddr0 = (unsigned long)timer0_isr;
    VICVectCntl0 = 0x20 | TIMER0_IRQ_NUM;  // Enable slot & assign interrupt source (bit 5 set)

    // Enable Timer0 interrupt in VIC
    VICIntEnable = (1 << TIMER0_IRQ_NUM);
}

int main(void) {
	
    init_vic();   // Setup interrupt controller

    while (1) {
        // Clear flag
        interrupt_flag = 0;

        // Trigger software interrupt
        VICSoftInt = (1 << TIMER0_IRQ_NUM);

        // Wait for ISR to set the flag
        while (!interrupt_flag);

        // Simulate post-ISR task (toggle LED or send message)
        // Here just a delay and re-trigger for testing
        delay_ms(1000);

        // Clear software interrupt if needed
        VICSoftIntClear = (1 << TIMER0_IRQ_NUM);
    }
}
