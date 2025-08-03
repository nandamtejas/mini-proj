// switch_test.c

/*
	This program detects the switch press and executes the following operations
	- if switch is pressed, LED turns ON
	- if switch is released, LED turns OFF
	
	*** Using Active LOW Switch and Active LOW LED ***
*/

// we are using LPC2148
#define __LPC2148

#ifndef __LPC2148
	#include <LPC21xx.h>
#else
	#include <LPC214x.h>
#endif

#include "../types.h"
#include "../delay.h"
#include "../defines.h"

// defines for switch and LED
#define SW_AL_0_3 3 // Switch @P0.3 (PORT 0 pin 3)
#define LED_AL_1_16 16 // LED @P1.16 (PORT 1 pin 16)

// function for reading switch status
uint32 readSwitch(uint32 switchPin)
{
	return READBIT(IOPIN0, switchPin);
}

int main()
{
	// Cfg LED pin (@P1.16) as gpio output pin
	WRITEBIT(IODIR1, LED_AL_1_16, 1);
	
	while(1)
	{
		// check for the switch status
		// if switch is pressed -> LED should glow (turn ON)
		while(readSwitch(SW_AL_0_3) == 0) // Active LOW
		{
			// Turn ON LED (Active LOW)
			WRITEBIT(IOPIN1, LED_AL_1_16, 0);
		}
		
		// if switch released -> LED should be turned OFF
		while(readSwitch(SW_AL_0_3) == 1)
		{
			// Turn OFF LED (Active LOW)
			WRITEBIT(IOPIN1, LED_AL_1_16, 1);
		}
	}
}
