// messages.h

#include "types.h"

#ifndef _MESSAGES_H
#define _MESSAGES_H
#include "messages_defines.h"

void getMessagesFromTime(uint32 hour, uint32 min);
void displayMarquee(void);
void setNextMessageAlarm(void);
void edit_message(void);
uint32 isCurrentMessagesModifiedorCorrupted(void);
void reorderCurrentMessages(void);
int32 getCurrentMessagesLength(void);

#endif
