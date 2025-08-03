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
