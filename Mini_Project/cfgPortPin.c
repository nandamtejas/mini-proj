// cfgPortPin.c

#include "MCU_settings.h"
#include "mainLPCheader.h"
#include "types.h"
#include "defines.h"

void cfgPortPin(uint32 port, uint32 pinNo, uint32 pinFunc)
{
	// This function configures the port pins work as specific function by specifying pinFunc which ranges from (0 to 3) in PINSEL(0/1/2) function.
	// LPC2129 and LPC2148 has only two ports (PORT0 (30 pins) and PORT1(16 pins) )
	// for PORT0 cfg PINSEL0 or PINSEL1 register
	// for PORT1 cfg PINSEL2 register
	
	// As only PORT0 containing alternating functions, PINSEL(0/1) needs to be configured corresponding to pinNo
	// PINSEL0 responsible for cfg for pin(0-15) (each 2 bit represents 1 pin function; default val is 00 -> GPIO func)
	// PINSEL1 responsible for cfg for pin(16-31) (each 2 bit represents 1 pin function; default val is 00 -> GPIO func)

	if (port == 0)
	{
		// if pinNo ranges from 0-15, Cfg PINSEL0 SFR
		if (pinNo<16)
		{
			//WRITENIBBLE(PINSEL0, (pinNo*2), pinFunc);
			PINSEL0 = ((PINSEL0 &~ (3<<(pinNo*2))) | (pinFunc<<(pinNo*2)));
		}
		else if ((pinNo>=16) && (pinNo<32))
		{
			//WRITENIBBLE(PINSEL1, ((pinNo-16)*2), pinFunc);
			PINSEL1 = ((PINSEL1 &~ (3<<((pinNo-16)*2))) | (pinFunc<<((pinNo-16)*2)));
		}
	}
}
