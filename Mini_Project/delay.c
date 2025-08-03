// delay.c

/*
This file is for delat definitions
*/
#include "types.h"

void delay_us(uint32 dlyUS)
{
	dlyUS *= 12;
	while(dlyUS--);
}

void delay_ms(uint32 dlyMS)
{
	dlyMS *= 12000;
	while(dlyMS--);
}

void delay_s(uint32 dlyS)
{
	dlyS *= 12000000;
	while(dlyS--);
}
