// main.c
/*
MAIN PROGRAM:

@brief
# EVENT DISPLAY BOARD

This embedded application is designed for `LPC2148` to implement a real-time clock system:`RTC` with temperature display:`ADC`,
message scrolling (event board), and administrative control. The system operates in three primary modes:

1. `CLOCK_MODE` (Default):
   - Displays current time and temperature on a 16x2 LCD.
   - A status LED at P1.25 indicates active clock mode.

2. `MESSAGE_SCROLL_MODE` (Triggered by RTC Alarm):
   - Activates for 15 minutes when the RTC alarm triggers.
   - Retrieves scheduled messages based on current time and scrolls them on the LCD.
   - Can be terminated early if time is modified in ADMIN_MODE.
   - A status LED at P1.26 indicates active message mode.
   - This mode operates under the control of three defined alarm states:

        - `ALARM_DISABLE`:
      Indicates that the alarm is currently inactive. No message scrolling takes place in this state.

        - `ALARM_START_MODE`:
      Entered when an alarm is triggered via interrupt. This state transitions from `ALARM_DISABLE`.
      During this mode, the system starts scrolling messages on the LCD.
      A stop-alarm is scheduled 15 minutes after the start to automatically end the scrolling session.

        - `ALARM_STOP_MODE`:
      Triggered when the stop-alarm set during ALARM_START_MODE is activated.
      This mode signals the end of the message scrolling period.
      The system then transitions back to CLOCK_MODE.

3. `ADMIN_MODE` (Triggered by EINT0 interrupt):
   - Allows password-protected access for editing messages or RTC time.
   - Temporarily disables alarms and scrolls.
   - Ensures correct alarm reconfiguration if time/messages are updated.

The system uses interrupts for both external triggers (admin access) and real-time alarms.
`scrollStopFlag` is used globally to control the scrolling process from within the alarm and admin routines.
The system leverages structured enums, modular peripherals, and volatile flags to coordinate state transitions.

Hardware components involved include:
- RTC module
- External Interrupt <u>(EINT0)</u>
- LM35 temperature sensor
- <B>16x2</B> Character LCD
- 4x4 Keypad Matrix (KPM)
- 2 Status LED's
- Keil uVision + Proteus Simulation support

*/


#include "MCU_settings.h"
#include "mainLPCheader.h"
#include "types.h"
#include "delay.h"
#include "defines.h"
#include "cfgPortPin.h"
#include "lm35.h"
#include "lcd.h"
#include "rtc.h"
#include "kpm.h"
#include "kpm_admin.h"
#include "ISR.h"
#include "InterruptCfg.h"
#include "messages.h"
#include "message_marquee.h"



// LCD CGRAM
const uint8 degCentSymbol[8] = {0x04,0x0A,0x0A,0x04,0x00,0x00,0x00,0x00};
// const uint8 degCentSymbol[8] = {0x08,0x14,0x0B,0x04,0x04,0x04,0x03,0x00};
// const uint8 degCentSymbol[8] = {0x17,0x08,0x08,0x08,0x08,0x08,0x07,0x00};

// enum for SystemModes
// CLOCK_MODE -> (Default/Main mode) This mode shows the RTC Clock with temperature display
// MESSAGE_SCROLL_MODE -> This mode is triggered with Alarm, Scrolls the messages in this mode for 15 minutes
// ADMIN_MODE -> This mode is responsible for RTC modifying or messages handling Authenticated with password.
typedef enum {
	CLOCK_MODE,
	MESSAGE_SCROLL_MODE,
	ADMIN_MODE
} SystemMode;

// Alarm flag enum
// ALARM_START_MODE -> (Default Mode) This mode indicates the Message scrolling process to start
// ALARM_STOP_MODE -> This mode indicated the Message scrolling process to stop
typedef enum {
	ALARM_DISABLE,
	ALARM_START_MODE, 
	ALARM_STOP_MODE
} AlarmMode;

// define current mode as CLOCK_MODE
volatile SystemMode currentSystemMode = CLOCK_MODE;
// define previous mode 
volatile SystemMode previousSystemMode;


// Define current alarm mode as ALARM_START_MODE
volatile AlarmMode currentAlarmMode = ALARM_DISABLE;

// flag to stop scrolling
// 0 -> Condition to continue the process
// 1 -> Condition to stop the process
volatile uint32 scrollStopFlag = 0;

// variable to hold the message modified status
// 0 -> Condition indicating message not modified
// 1 -> Condition indicating message modified
volatile uint32 msgModifiedFlag = 0;

// ISR declaration
// ADMIN_MODE eint0 isr
void eint0_isr(void) __irq
{
	//ISR Activity starts
	// Check if the currentSystemMode is already ADMIN_MODE, if true do nothing
	if (ISEQUAL(currentSystemMode, ADMIN_MODE)) goto EXIT_EINT0;
	
	// If the currentSystemMode is ADMIN_MODE, No other interrupt should occur, Hence Alarm Interrupt is disables for a while
	VICIntEnClr = (1<<ALARM_VIC_CHNO);
	
	// Turn off the other LED
	IOCLR1 = 1<<CLOCK_MODE_STATUS_LED;
	IOCLR1 = 1<<MESSAGE_SCROLL_MODE_STATUS_LED;
	
	// Change currentSystemMode
	previousSystemMode = currentSystemMode;
	currentSystemMode = ADMIN_MODE;
	
	// if previousSystemMode is MESSAGE_SCROLL_MODE, set to 1
	if ((previousSystemMode == MESSAGE_SCROLL_MODE) && (currentAlarmMode == ALARM_START_MODE))
	{
		// stop scrolling
		scrollStopFlag=1;
	}
	
	// ISR Activity ends
EXIT_EINT0:	EXTINT = (1<<0);
	
	// Clear the VICVectAddr register
	VICVectAddr = 0;
}

// Alarm register ISR
void rtc_alarm_isr(void) __irq
{	
	// ISR Activity Starts
	
	// Check if the Alarm triggered mistakenly
	//if (!(currentTimeWithinMessageSchedule())) goto EXT_RTC;
	
	// Change currentSystemMode to MESSAGE_SCROLL_MODE
	previousSystemMode = currentSystemMode;
	currentSystemMode = MESSAGE_SCROLL_MODE;
	
	// Clear other status LED
	IOCLR1 = 1<<CLOCK_MODE_STATUS_LED;
	
	// If currentSystemMode is ADMIN_MODE
	// check if ISR invoked later it should be invoked
	if ( ISWITHIN(MIN, ALMIN, ((ALMIN+15)-((ALMIN+15)%15)) ))
	{
		// Check for the currentAlarmMode
		if (currentAlarmMode == ALARM_DISABLE)
		{
			// If currentAlarmMode is ALARM_DISABLE, Set a next alarm after 15 minutes to stop  and mode set to ALARM_START_MODE
			ALMIN = (MIN + 15) - ((MIN+15)%15);											    
			ALHOUR = (HOUR + (ALMIN / 60)) % 24;
			ALMIN = ALMIN%60;
			currentAlarmMode = ALARM_START_MODE;
			// disable the scrollStopFlag
			scrollStopFlag = 0;
		}
		else if (currentAlarmMode == ALARM_START_MODE)
		{
			// If currentAlarmMode is ALARM_START_MODE, get the next alarm details and modify in Alarm Register. 
			setNextMessageAlarm();
			// enable the scrollStopFlag
			scrollStopFlag = 1;
			// set the Alarm Mode to ALARM_STOP_MODE
			currentAlarmMode = ALARM_STOP_MODE;
		}
	}	
	
	// ISR Activuty complete
	// Clear Interrupt Enable
	ILR = 1<<1;
	
	// Clear the VICVectAddr register
	VICVectAddr = 0;
}

int main()
{	
	volatile uint32 flag=0;
	// uint32 prevALHour, prevALMin;
	uint32 currMins, startMins, stopMins;
	// Cfg @P1.25, P1.26, P1.27 as Gpio out pins
	WRITENIBBLE(IODIR1, CLOCK_MODE_STATUS_LED, 3);
	
	// Init RTC
	InitRTC();
	
	// Init KPM
	InitKPM();
	
	// Init Interrupt
	InitInterrupt();

	//setTime(7, 44, 58);
	
	// Set date and time for testing purpose
	// setDate(8,8,2025);
	// setTime(7,44,58);
	
	// Init LCD
	InitLCD();
	
	// BuildCGRAM
	BuildCGRAM_v2(degCentSymbol, 0);
	CmdLCD(DSP_ON_CUR_OFF);
	delay_ms(250);
	CmdLCD(CLEAR_LCD);
	
	// Check current time is within the schedule
	if (currentTimeWithinScrollWindow())
	{
		// set Alarm
		setAlarmTime(HOUR, MIN);
	}
	else
	{
		// Initial Alarm Set
		setNextMessageAlarm();
	}
	
	// 
	while(1)
	{
		switch(currentSystemMode)
		{
			// CLOCK_MODE operation
			case CLOCK_MODE: // Main Clock mode
				// CLOCK_MODE status LED ON
				IOSET1 = 1<<CLOCK_MODE_STATUS_LED;
				// Preffer cursor off for displaying time 
				CmdLCD(DSP_ON_CUR_OFF);
				CmdLCD(GOTO_LINE1_POS0);
				
				// display clock
				displayClock();
				// display temperature
				displayTemp();
				//previousSystemMode = CLOCK_MODE;
				break;
			
			// MESSAGE_SCROLL_MODE operation
			case MESSAGE_SCROLL_MODE: // Message mode (IRQ interrupt)
				// is done only when currentAlarmMode is ALARM_START_MODE
				
				// MESSAGE_SCROLL_MODE status LED ON
				IOSET1 = 1<<MESSAGE_SCROLL_MODE_STATUS_LED;
				CmdLCD(CLEAR_LCD);
				
					while (currentAlarmMode == ALARM_START_MODE)
					{
						// Check for scroll stop flag
						if (scrollStopFlag==1)
							break;
						// In ALARM_START_MODE, message scrolling process will be done	
						CmdLCD(DSP_ON_CUR_OFF);						
						// collect available messages
						if (flag == 0)
						{
							getMessagesFromTime(HOUR, MIN);
							flag = 1;
						}
						// display messages in marquee
						displayMarquee();
						// delay_ms(150);
					}
					if (currentAlarmMode == ALARM_STOP_MODE)
					{
						// In ALARM_STOP_MODE (i.e after 15 minutes of Alarm triggered) the process should be stopped 
						// transit currentAlarmMode to ALARM_DISABLE
						currentAlarmMode = ALARM_DISABLE;
						flag=0;
					}
					
					// check if the Alarm stopped due to Admin mode
					if (currentSystemMode == ADMIN_MODE)
					{	
						// reset and switch mode
						flag=0;
						break;
					}
				
				// Change currentSystemMode to CLOCK_MODE
				previousSystemMode = MESSAGE_SCROLL_MODE;
				currentSystemMode = CLOCK_MODE;
				
				// MESSAGE_SCROLL_MODE status LED OFF
				IOCLR1 = 1<<MESSAGE_SCROLL_MODE_STATUS_LED;
				// Refresh the screen
				CmdLCD(CLEAR_LCD);
				break;
			
			// ADMIN_MODE operation
			case ADMIN_MODE: // Admin Mode (FIQ interrupt)
				
				// Refresh the screen
				CmdLCD(CLEAR_LCD);
				adminMode();
			
				// For either MESSAGE_SCROLL_MODE or CLOCK_MODE, The LCD Cursor should be OFF
				CmdLCD(CLEAR_LCD);
				CmdLCD(DSP_ON_CUR_OFF);
				
				// If the previousSystemMode is MESSAGE_SCROLL_MODE, display the EVENT_BOARD
				// Re-display the EVENT-BOARD in LCD
				if ((previousSystemMode == MESSAGE_SCROLL_MODE) && (currentAlarmMode == ALARM_START_MODE))
				{
					// Check if the RTC time has modified by the ADMIN
					// prevALMin = ((ALMIN-15)+60)%60;
					// prevALHour = (ALMIN<=15)?ALHOUR-1:ALHOUR;
					stopMins = ALHOUR*60+ALMIN;
					startMins = stopMins-15;
					currMins = HOUR*60+MIN;
					
					if ( 
						(!currentTimeWithinScrollWindow())
						|| ((msgModifiedFlag == 1) && (getCurrentMessagesEnabledLength() == 0))
					)
					{
						// If the time has been changed by the ADMIN which is not between any of the 
						// range of ALARM_START time and ALARM_STOP time (15 minutes Bandwidth), 
						// Or the message has beed modified
						// Then reset the currentAlarmMode to ALARM_STOP_MODE
						
						//StrLCD("Not WITHIN");
						//delay_s(1);

						currentAlarmMode = ALARM_STOP_MODE;
						// reset the next alarm
						setNextMessageAlarm();
						// stop current scrolling
						scrollStopFlag = 1;
					}
					else if ( currentTimeWithinScrollWindow() )
					{
						//if (ISWITHIN(HOUR, prevALHour, ALHOUR) && ISWITHIN(MIN, prevALMin, ((ALMIN<15)?(ALMIN+60):(ALMIN))))
						if (ISWITHIN(currMins, startMins, stopMins))
						{
							// if the current time is changed within the current scroll time interval, i.e, in the range of ALARM_START and ALARM_STOP
							// for example if system mode is under message scroll starts from 7:45 and admin triggered and changed the time to 7:55,
							// which is between the current message interval, continue scrolling
							// set stop scroll flag to 0 for continuing scrolling
							//StrLCD("WITHIN SAME");
							//delay_s(1);
							scrollStopFlag=0;
							// Refresh the EVENT BOARD title
							CmdLCD(GOTO_LINE2_POS0);
							StrLCD("EVENT BOARD");
							CmdLCD(GOTO_LINE1_POS0);
						}
						else
						{
							// if the current time might not be in the current message scroll range, 
							// but it is in other message schedule range,
							// for example if the admin changed the time from 7:55 (message scroll interval time) to 10:25 (also the message scroll interval time for another message)
							// Then reset with new messages with new Alarm.
							//StrLCD("WITHIN DIFF");
							//delay_s(1);
							currentAlarmMode = ALARM_DISABLE;
							flag=0;
							setAlarmTime(HOUR, MIN);
						}
					}				
				}
				
				// reset the message modified flag
				msgModifiedFlag=0;
				
				
				// If any changes made for Date and Time or Message, Make Sure to update the Alarm Details which should be go along with current time
				// If Time has changed or any of message enable bit is modified, call the below function
				// This function should be called when Alarm is disabled, i.e; If Message Scrolling is happening right now, This function should not be called
				if (currentAlarmMode == ALARM_DISABLE)
				{
					// If the time modified by ADMIN which lies within the interval of message schedule time, then set Alarm now
					if (currentTimeWithinScrollWindow())
					 {
						// set alarm and exit
						setAlarmTime(HOUR, MIN);
					 }
					 else
						setNextMessageAlarm();
				}
				
				// reset the status LEDs
				IOCLR1 = ((1<<CLOCK_MODE_STATUS_LED) | (1<<MESSAGE_SCROLL_MODE_STATUS_LED));
				
				if (previousSystemMode == CLOCK_MODE)
					IOSET1 = 1<<CLOCK_MODE_STATUS_LED;
				else if (previousSystemMode == MESSAGE_SCROLL_MODE)
					IOSET1 = 1<<MESSAGE_SCROLL_MODE_STATUS_LED;
				
				currentSystemMode = previousSystemMode;
				previousSystemMode = ADMIN_MODE;
				CmdLCD(DSP_ON_CUR_OFF);
				
				// Re-Enable the Alarm register
				VICIntEnable = (1<<ALARM_VIC_CHNO);
				break;
		}
	}
}
