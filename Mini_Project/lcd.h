// lcd.h
#include "types.h"

#ifndef _LCD_H
#define _LCD_H
#include "lcd_defines.h"

void WriteLCD(uint8);
void CmdLCD(uint8);
void InitLCD(void);
void CharLCD(uint8);
void StrLCD(uint8 *);
void U32LCD(uint32);
void S32LCD(int32);
void F32LCD(real32, uint32);
void BuildCGRAM(const uint8*, uint32);
void BuildCGRAM_v2(const uint8*, uint32);
void BuildCGRAMLUT(const uint8 (*p)[8], uint32);

#endif
