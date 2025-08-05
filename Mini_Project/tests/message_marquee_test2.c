// message_marquee_test2.c


#include "../MCU_settings.h"
#include "../mainLPCheader.h"

#include "../types.h"
#include "../delay.h"
#include "../defines.h"
#include "../message_marquee.h"
#include "../lcd_defines.h"
#include "../lcd.h"
#include "../messages.h"


//extern Message messageList;
extern Message *currentMessages[10];
volatile uint32 scrollStopFlag=0;

int main()
{
	uint32 hour=9, minute=45;
	uint32 len,i;
	
	InitLCD();
	
	getMessagesFromTime(hour, minute);
	len=getCurrentMessagesLength();
	
	for (i=0; i<len; i++)
	{
		message_marquee_v3(currentMessages[i]->text);
	}
}
