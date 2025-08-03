// delay_test.c

// we are using LPC2148
#define __LPC2148

#include "../mainLPCheader.h"

#include "../types.h"
#include "../delay.h"
#include "../defines.h"

// LED Blink for every half second
#define DELAYMS 500

// LED connected in port 0 pin 1
#define LED_AL_0_1 1 // LED @P0.1

int main()
{
	// Cfg P0.1 or LED pin as gpio out pin
	WRITEBIT(IODIR0, LED_AL_0_1, 1);
	
	while(1)
	{
		WRITEBIT(IOPIN0, LED_AL_0_1, !(READBIT(IOPIN0, LED_AL_0_1)));
		delay_ms(DELAYMS);
	}
}
