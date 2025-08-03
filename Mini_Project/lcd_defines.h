// lcd_defines.h

#ifndef _LCD_DEFINES_H
#define _LCD_DEFINES_H

#define CLEAR_LCD 0x01
#define RET_CUR_HOME 0x02

#define ENTRY_CUR_RIGHT 0x06
#define ENTRY_CUR_LEFT 0x07

#define DSP_OFF 0x08
#define DSP_ON_CUR_OFF 0x0C
#define DSP_ON_CUR_NOBLINK 0x0E
#define DSP_ON_CUR_BLINK 0x0F

#define SHIFT_CUR_LEFT 0x10
#define SHIFT_CUR_RIGHT 0x14

#define SHIFT_DSP_LEFT 0x18
#define SHIFT_DSP_RIGHT 0x1C

#define MODE_4BIT_1LINE 0x20
#define MODE_4BIT_2LINE 0x28
#define MODE_8BIT_1LINE 0x30
#define MODE_8BIT_2LINE 0x38

#define GOTO_LINE1_POS0 (0x80 + 0x00)
#define GOTO_LINE2_POS0 (0x80 + 0x40)
#define GOTO_LINE3_POS0 (0x80 + 0x14)
#define GOTO_LINE4_POS0 (0x80 + 0x54)

#define GOTO_CGRAM_START 0x40

#define LCD_DATA 8 // LCD Data Lines @P0.8 (D0) to @P0.15 (D7)
#define LCD_RS 16 // Regster Select (RS) @P0.16
#define LCD_RW 17 // ReadWrite Pin @P0.17 (Always send 0 as there is no scope to read data from LCD mostly)
#define LCD_EN 18 // LCD Enable pin @P0.18

#endif
