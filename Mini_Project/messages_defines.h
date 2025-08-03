// messages_defines.h

#ifndef _MESSAGES_DEFINES_H
#define _MESSAGES_DEFINES_H

#include "types.h"

typedef struct {
	uint32 hour;
	uint32 minute;
	uint8 text[80];
	uint8 enabled;
} Message;

#endif
