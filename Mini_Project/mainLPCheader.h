// mainLPCheader.h

/*
	This headerfile chooses the microcontroller device
*/

#ifndef __MAINLPCHEADER_H
#define __MAINLPCHEADER_H

#ifdef __LPC2129
    #include <LPC21xx.h>
#elif defined(__LPC2148)
    #include <LPC214x.h>
#else
    #error "Please define a supported LPC microcontroller macro (e.g., __LPC2129 or __LPC2148)"
#endif

#endif
