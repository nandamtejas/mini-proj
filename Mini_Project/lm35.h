// lm35.h

#include "types.h"

#ifndef _LM35_H
#define _LM_35_H

#include "ADC_defines.h"
#include "ADC.h"
#include "lm35_defines.h"

void ReadLM35(real32 *temp);
void displayTemp(void);

#endif
