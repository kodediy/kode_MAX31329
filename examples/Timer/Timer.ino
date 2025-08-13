/**
 * MAX31329 Timer interrupt demo: configures countdown timer to trigger at 16Hz with auto-repeat.
 * Uses hardware timer functionality with interrupt handling for precise timing on ESP32-S3.
 * Demonstrates periodic timer events with timestamped notifications.
 */
/* ───────── KODE | docs.kode.diy ───────── */

#include <MAX31329.h>

MAX31329 rtc;
volatile bool timerFlag = false;
const int pinInterrupt = 2; /* GPIO pin connected to MAX31329 timer interrupt output */

/* Timer Interrupt Service Routine - must be in IRAM for ESP32-S3 */
static void IRAM_ATTR onInterrupt()
{
	timerFlag = true;
}

void setup()
{
	Serial.begin(115200);
	Serial.println("MAX31329 Timer example");
	
	/* Configure timer interrupt pin with pull-up resistor */
	pinMode(pinInterrupt, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(pinInterrupt), onInterrupt, FALLING);

	/* Initialize RTC - configure Wire.begin(48,47) for kode dot hardware */
	rtc.begin();

	/* Set initial time using fluent API */
	rtc.t.year = 2024;
	rtc.t.month = 11;
	rtc.t.day = 24;
	rtc.t.hour = 15;
	rtc.t.minute = 10;
	rtc.t.second = 0;
	rtc.t.dayOfWeek = 0; /* Sunday */
	rtc.writeTime();

	/* Configure countdown timer: initial_value=16, repeat=true, frequency=16Hz */
	if (!rtc.timerConfigure(16, true, 3)) {  /* freqSel: 0=1024Hz, 1=256Hz, 2=64Hz, 3=16Hz */
		Serial.println("timerConfigure failed");
	}
	
	/* Enable timer interrupt in RTC */
	rtc.enableInterrupts(MAX31329_INT_TIE);

	/* Start the countdown timer */
	rtc.timerStart();
}

void loop()
{
	/* Handle timer interrupt when it occurs */
	if (timerFlag) {
		timerFlag = false;  /* Reset interrupt flag */
		
		uint8_t st;
		/* Verify Timer Interrupt Flag is set in status register */
		if (rtc.readStatus(st) && (st & MAX31329_STATUS_TIF)) {
			/* Read and display current time when timer fires */
			if (rtc.readTime()) {
				Serial.printf("TIMER: %04d-%02d-%02d %02d:%02d:%02d\n",
					rtc.t.year, rtc.t.month, rtc.t.day,
					rtc.t.hour, rtc.t.minute, rtc.t.second);
			}
			/* Clear timer interrupt flag - timer auto-restarts due to repeat=true */
			rtc.clearStatus();
		}
	}
}
