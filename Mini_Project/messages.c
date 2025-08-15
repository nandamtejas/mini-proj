// messages.c

#include "MCU_settings.h"
#include "mainLPCheader.h"
#include "types.h"
#include "delay.h"
#include "defines.h"
#include "messages_defines.h"
#include "message_marquee.h"
#include "lcd.h"
#include "rtc.h"
#include "kpm.h"

// Total messages
#define TOTAL_MESSAGES 10

Message messageList[TOTAL_MESSAGES] = {
	{7, 45, "Good Morning! Classes Starts Soon", 1},
	{13, 45, "C Programming Session in Class number 2", 1},
	{10, 15, "C Module theory exam in 4th floor lab1", 1},
	{10, 15, "C Module lab exam in 4th floor lab2 and middle lab at 10:30AM", 1},
	{12, 45, "Lunch Break from 1PM-2PM", 1},
	{9, 45, "ARM Workshop on external interrupts in LAB1 at 10AM", 1},
	{9, 45, "ARM-KIT issue time from 10AM - 10:30AM in the middle lab", 1},
	{15, 15, "Only 15 mins break for next ARM session", 1},
	{17, 0, "Revise Today's class programs at home!", 1},
	{17, 45, "End of Day - See you Tommorow!", 1},
};

// Function to clear the current Messages 
Message emptyMessage = {0,0,"",1};
	
// Considering currentMessages as Array of pointer to Message structure
Message *currentMessages[10];
int32 sel=0;

// clear current Messages
void clearCurrentMessages(void)
{
	for (--sel; sel>=0; sel--)
	{
		// overwrite the message index with empty
		currentMessages[sel] = &emptyMessage;
	}
	// reset the sel value
	sel=0;
}

// check current time is in between 15 minutes interval of any message
uint32 isWithinScrollWindow(uint32 hour, uint32 min, Message message)
{
	// This function checks whether the current time is in between the message intervals or not
	// for eg: current time is 7:50, and the messageList has message scheduled at 7:45
	// as current time is in the interval between 7:45 and 8:00, it should return 1 otherwise 0
	/*uint32 lbhour, ubhour;
	uint32 lbmin, ubmin;
	
	lbmin = message.minute; 
	ubmin = message.minute+15;
	lbhour = message.hour; 
	ubhour = (ubmin>=60)?lbhour+1: lbhour;
	
	if (ISWITHIN(hour, lbhour, ubhour))
		if (ISWITHIN(min, lbmin, ubmin))
			return 1;
	return 0;
	*/
	uint32 currMinutes = hour*60+min;
	uint32 startMinutes = message.hour*60+message.minute;
	uint32 stopMinutes = startMinutes+15;
	
	// Handle wrap around at midnight
	if (stopMinutes>=24*60)
		stopMinutes -= 24*60;
	
	if (startMinutes <= stopMinutes)
	{
		// Normal day case
		return ISINRANGE(currMinutes, startMinutes, stopMinutes);
	}
	else
	{
		// Wrap-around case: window goes past midnight
		return (ISGTEQ(currMinutes, startMinutes) || ISLESSER(currMinutes, stopMinutes));
	}
}

// check time is within the messages
uint32 currentTimeWithinScrollWindow(void)
{
	// This function checks the current time within the message schedules
	uint32 i;
	
	for (i=0; i<TOTAL_MESSAGES; i++)
	{
		// check all the messages whether the current time is within the interval,
		// if yes, return 1, otherwise return 0
		if ((messageList[i].enabled) && isWithinScrollWindow(HOUR, MIN, messageList[i]) )
		{
			return 1;
		}
	}
	return 0;
}

// get messages from time
void getMessagesFromTime(uint32 hour, uint32 min)
{
	// This function returns the Messages which are enabled
	uint32 i;
	
	// First clear the previous messages
	if (sel>0)
		clearCurrentMessages();
	
	for (i=0; i<TOTAL_MESSAGES; i++)
	{
		/*// Check hour and min match
		if (ISEQUAL(messageList[i].hour, hour) && ISEQUAL(messageList[i].minute, min))
		{
			// Check if the message is enabled
			if (messageList[i].enabled == 1)
			{
				// include this message
				currentMessages[sel++] = &messageList[i];
			}
		}
		*/
		
		// check if message is enabled or disabled
		if (messageList[i].enabled == 1)
		{
			// if message is enabled, further check whether the time is in the range of 15 minutes
			if (isWithinScrollWindow(hour, min, messageList[i]))
			{
				// if current time is within message interval, add message in currentMessageScroll
				currentMessages[sel++] = &messageList[i];
			}
		}
	}
}

// editMessageByIndex
void editMessageByIndex(uint32 index, uint32 status)
{
	// This function is used to edit the message in messageList of given 'index' with 'status'
	messageList[index].enabled = status;
}

// enableOrDisableMessage
void enableOrDisableMessage(uint32 index)
{
	// This function supports the option to support whether to enable or disable the status message
	uint8 key;
	uint32 status;
	// Clear LCD 
AFT_PASS: CmdLCD(CLEAR_LCD);
	StrLCD("1. Enable");
	CmdLCD(GOTO_LINE2_POS0);
	StrLCD("2. Disable");
	
	// turn on cursor
	CmdLCD(DSP_ON_CUR_NOBLINK);
	
	key = keyScan();
	delay_ms(100);
	switch(key)
	{
		case '1': // enable
					status=1; break;
		case '2': // disable
					status=0; break;
		default: // repeat until it gives correct pin
				CmdLCD(CLEAR_LCD);
				CmdLCD(DSP_ON_CUR_OFF);
				StrLCD("Incorrect option");
				delay_ms(50);
				goto AFT_PASS;
	}
	
	// editMessageByIndex
	editMessageByIndex(index, status);
}

// edit message
void edit_message(void)
{
	// Edit message function for Admin use
	uint8 key;
	uint32 index=0;

	// Clear LCD 
AFT_PASS: CmdLCD(CLEAR_LCD);
	StrLCD("Enter the ");
	CmdLCD(GOTO_LINE2_POS0);
	StrLCD("message id(0-9): ");
	
	// turn on cursor
	CmdLCD(DSP_ON_CUR_NOBLINK);
	
	// Get KPM key input
	key = keyScan();
	delay_ms(100);
	switch(key)
	{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9': // entered correct index
					// next step is to edit message
					// edit messageList of index either to enable or disable the status of message
					index = (key-'0');
					enableOrDisableMessage(index);
					break;
		default:// turn off the cursor
			CmdLCD(CLEAR_LCD);
			CmdLCD(DSP_ON_CUR_OFF);
			StrLCD("Incorrect Index");
			delay_ms(50);
			goto AFT_PASS;
			
	}
}


// display message_marquee
void displayMarquee(void)
{
	// This function provides the logic for message marquee
	int32 i;
	// Function to dipslay message marquee
	//CmdLCD(CLEAR_LCD);
	CmdLCD(GOTO_LINE2_POS0);
	StrLCD("EVENT BOARD");
	CmdLCD(GOTO_LINE1_POS0);
	
	// call message marquee function
	for (i=0; i<sel; i++)
	{
		// call message marquee function
		if (currentMessages[i]->enabled == 1)
		{
			message_marquee_v3(currentMessages[i]->text);
		}
	}
}

int32 getCurrentMessagesEnabledLength(void)
{
	// function to get no of currentMessages Enabled, which was modified/corrupted by ADMIN
	uint32 i, len=0;
	for (i=0; i<sel; i++)
	{
		// check if currentMessages[i] is enabled
		if (currentMessages[i]->enabled == 1)
			len++;
	}
	// return the len
	return len;
}



// Function to get next message's hour and min to be displayed in message scroll
void setNextMessageAlarm(void)
{
	// Function to set next message's Alarm
	uint32 hour=24, minute=60;
	
	// iterate through all the messages in messageList and check for hour and min with current Time. 
	// If a message.hour and message.min if greater than the current time hour and min, then set that hour and min as Alarm
	int32 i=0;
	for (i=0; i<TOTAL_MESSAGES; i++)
	{
		// check message is enabled or not
		if (messageList[i].enabled == 1)
		{
			if (ISLESSER(messageList[i].hour, hour) || (ISEQUAL(messageList[i].hour, hour) && ISLTEQ(messageList[i].minute, minute)))
			{
				if (ISLESSER(HOUR, messageList[i].hour) || (ISEQUAL(HOUR, messageList[i].hour) && ISLTEQ(MIN, messageList[i].minute)))
				{
					// update the hour and minute
					hour = messageList[i].hour;
					minute= messageList[i].minute;
				}
			}
		}
	}
	
	// set the Alarm
	if ((hour == 24) && (minute == 60)) 
	{
		// if the current time is not in any scroll window, then set the Alarm at the starting time.
		// logic to find starting time
		for (i=0; i<TOTAL_MESSAGES; i++)
		{
			if (ISLESSER((messageList[i].hour*60+messageList[0].minute), (hour*60+minute) ))
			{
				hour = messageList[i].hour;
				minute = messageList[i].minute;
			}
		}
	}
	
	setAlarmTime(hour, minute);
	// make current messages to display be ready
	// getMessagesFromTime(hour, minute);
}

// 
