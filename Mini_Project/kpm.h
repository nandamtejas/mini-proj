// kpm.h

#include "types.h"

#ifndef _KPM_H
#define _KPM_H

#include "kpm_defines.h"

void InitKPM(void);
uint32 colScan(void);
uint32 rowCheck(void);
uint32 colCheck(void);
uint8 keyScan(void);

// user input function
void strKeyScan(uint8*, uint32);
uint32 U32KeyScan(void);

#endif
