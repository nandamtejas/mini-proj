// main.c

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

// Defines for Mode Status LED
#define CLOCK_MODE_STATUS_LED 25 // @P1.25
#define MESSAGE_SCROLL_MODE_STATUS_LED 26 // @P1.26


// LCD CGRAM
const uint8 degCentSymbol[8] = {0x08,0x14,0x0B,0x04,0x04,0x04,0x03,0x00};
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

// ISR declaration
void eint0_isr(void) __irq
{
	uint32 prevALHour, prevALMin;
	// ISR Activity starts
	// Check if the currentSystemMode is already ADMIN_MODE, if true do nothing
	if (ISEQUAL(currentSystemMode, ADMIN_MODE)) return;
	
	// Turn off the other LED
	IOCLR1 = 1<<CLOCK_MODE_STATUS_LED;
	IOCLR1 = 1<<MESSAGE_SCROLL_MODE_STATUS_LED;
	
	// Change currentSystemMode
	previousSystemMode = currentSystemMode;
	currentSystemMode = ADMIN_MODE;
	
	// If the currentSystemMode is ADMIN_MODE, No other interrupt should occur, Hence Alarm Interrupt is disables for a while
	VICIntEnClr = (1<<ALARM_VIC_CHNO);
	// admin mode
	adminMode();
	CmdLCD(CLEAR_LCD);
	
	// If the previousSystemMode is MESSAGE_SCROLL_MODE, display the EVENT_BOARD
	// Re-display the EVENT-BOARD in LCD
	if ((previousSystemMode == MESSAGE_SCROLL_MODE) && (currentAlarmMode == ALARM_START_MODE))
	{
		// Check if the RTC time has modified by the ADMIN
		prevALMin = ((ALMIN-15)+60)%60;
		prevALHour = (ALMIN<=15)?ALHOUR-1:ALHOUR;

		if ( (ISNOTWITHIN(HOUR, prevALHour, ALHOUR)) || ISNOTWITHIN(MIN, prevALMin, ((ALMIN<15)?(ALMIN+60):(ALMIN))) )
		{
			// If the time has been changed by the ADMIN which is not between the range of ALARM_START time and ALARM_STOP time (15 minutes Bandwidth), 
			// Then reset the currentAlarmMode to ALARM_STOP_MODE
			currentAlarmMode = ALARM_STOP_MODE;
			// reset the next alarm
			setNextMessageAlarm();
			// stop current scrolling
			scrollStopFlag = 1;
		}
		else
		{
			CmdLCD(GOTO_LINE2_POS0);
			StrLCD("EVENT BOARD");
			CmdLCD(GOTO_LINE1_POS0);
		}
	}
			
	// If any changes made for Date and Time or Message, Make Sure to update the Alarm Details which should be go along with current time
	// If Time has changed or any of message enable bit is modified, call the below function
	// This function should be called when Alarm is disabled, i.e; If Message Scrolling is happening right now, This function should not be called
	if (currentAlarmMode == ALARM_DISABLE)
		setNextMessageAlarm();
	
	// reset the status LEDs
	if (previousSystemMode == CLOCK_MODE)
		IOSET1 = 1<<CLOCK_MODE_STATUS_LED;
	else if (previousSystemMode == MESSAGE_SCROLL_MODE)
		IOSET1 = 1<<MESSAGE_SCROLL_MODE_STATUS_LED;
	
	CmdLCD(DSP_ON_CUR_OFF);
		
	
	// Re-Enable the Alarm register
	VICIntEnable = (1<<ALARM_VIC_CHNO);
	
	// ISR Activity complete
	EXTINT = 1<<0;
	
	// Clear the VICVectAddr register
	VICVectAddr = 0;
}

void rtc_alarm_isr(void) __irq
{	
	// ISR Activity Starts
	
	// Change currentSystemMode to MESSAGE_SCROLL_MODE
	previousSystemMode = currentSystemMode;
	currentSystemMode = MESSAGE_SCROLL_MODE;
	
	// Clear other status LED
	IOCLR1 = 1<<CLOCK_MODE_STATUS_LED;
	
	// If currentSystemMode is ADMIN_MODE
	// check if ISR invoked later it should be invoked
	if (ISWITHIN(MIN, ALMIN, ALMIN+15))
	{
		// Check for the currentAlarmMode
		if (currentAlarmMode == ALARM_DISABLE)
		{
			// If currentAlarmMode is ALARM_DISABLE, Set a next alarm after 15 minutes to stop  and mode set to ALARM_START_MODE
			ALMIN = (MIN + 15) ;//- ((MIN+15)%15);											    
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
{	volatile uint32 flag=0;
	// Cfg @P1.25, P1.26, P1.27 as Gpio out pins
	WRITENIBBLE(IODIR1, CLOCK_MODE_STATUS_LED, 3);
	
	// Init RTC
	InitRTC();
	
	// Init KPM
	InitKPM();
	
	// Init Interrupt
	InitInterrupt();

	//setTime(7, 44, 58);
	
	// Initial Alarm Set
	setNextMessageAlarm();
	
	// Init LCD
	InitLCD();
	
	// BuildCGRAM
	BuildCGRAM_v2(degCentSymbol, 0);
	CmdLCD(DSP_ON_CUR_OFF);
	delay_ms(250);
	CmdLCD(CLEAR_LCD);
	
	// 
	while(1)
	{
		switch(currentSystemMode)
		{
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
				previousSystemMode = CLOCK_MODE;
				break;
			case MESSAGE_SCROLL_MODE: // Message mode (IRQ interrupt)
				// is done only when currentAlarmMode is ALARM_START_MODE
				
				// MESSAGE_SCROLL_MODE status LED ON
				IOSET1 = 1<<MESSAGE_SCROLL_MODE_STATUS_LED;
				CmdLCD(CLEAR_LCD);
				
					while (currentAlarmMode == ALARM_START_MODE)
					{
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
				
				// Change currentSystemMode to CLOCK_MODE
				previousSystemMode = MESSAGE_SCROLL_MODE;
				currentSystemMode = CLOCK_MODE;
				
				// MESSAGE_SCROLL_MODE status LED OFF
				IOCLR1 = 1<<MESSAGE_SCROLL_MODE_STATUS_LED;
				// Refresh the screen
				CmdLCD(CLEAR_LCD);
				break;
			case ADMIN_MODE: // Admin Mode (FIQ interrupt)
				
				//adminMode();
			
				// For either MESSAGE_SCROLL_MODE or CLOCK_MODE, The LCD Cursor should be OFF
				CmdLCD(DSP_ON_CUR_OFF);
				
				// Change currentSystemMode to previousSystemMode
				//currentSystemMode = (previousSystemMode == MESSAGE_SCROLL_MODE) ? MESSAGE_SCROLL_MODE: CLOCK_MODE;
				if ( (previousSystemMode == MESSAGE_SCROLL_MODE) && (currentAlarmMode == ALARM_START_MODE) )
				{
					currentSystemMode = MESSAGE_SCROLL_MODE;
					// Re-display the EVENT-BOARD in LCD
					CmdLCD(GOTO_LINE2_POS0);
					StrLCD("EVENT BOARD");
					CmdLCD(GOTO_LINE1_POS0);
				}	
					else if ( (previousSystemMode == MESSAGE_SCROLL_MODE) && (currentAlarmMode == ALARM_STOP_MODE) )
					{
						flag=0;
						currentSystemMode = MESSAGE_SCROLL_MODE;
					}
				else
					currentSystemMode = CLOCK_MODE;
					
						
				previousSystemMode = ADMIN_MODE;
				break;
		}
		
	}
}
