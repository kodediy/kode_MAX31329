/**
 * MAX31329 Alarm2 interrupt demo: configures Alarm2 to trigger every minute using simplified register setup.
 * Alarm2 has fewer fields than Alarm1 (MIN/HRS/DAY only) making configuration simpler on ESP32-S3.
 * Demonstrates alternative interrupt handling approach with different GPIO pin.
 */
/* ───────── KODE | docs.kode.diy ───────── */

#include <MAX31329.h>

MAX31329 rtc;
volatile bool alarm2Flag = false;
const int pinInterrupt = 3; /* GPIO pin for Alarm2 INT - different from Alarm1 for demo */

/* Interrupt Service Routine for Alarm2 - must be in IRAM */
static void IRAM_ATTR onInterrupt()
{
	alarm2Flag = true;
}

void setup()
{
	Serial.begin(115200);
	Serial.println("MAX31329 Alarm2 example");
	
	/* Configure interrupt pin for Alarm2 */
	pinMode(pinInterrupt, INPUT);
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

	/* Enable Alarm2 interrupt in RTC */
	rtc.enableInterrupts(MAX31329_INT_A2IE);

	/* Configure Alarm2 for every minute - Alarm2 only has MIN/HRS/DAY_DATE fields */
	uint8_t regs[3] = {0x80, 0x80, 0x80}; /* A2M2, A2M3, A2M4 masks set to ignore all fields */
	rtc.writeBytes(MAX31329_REG_ALM2_MIN, regs, sizeof(regs));

	/* Clear any pending alarm flags before starting */
	rtc.clearStatus();
}

void loop()
{
	/* Handle Alarm2 interrupt when it occurs */
	if (alarm2Flag) {
		alarm2Flag = false;  /* Reset interrupt flag */
		
		uint8_t st;
		/* Check if Alarm2 flag is actually set in status register */
		if (rtc.readStatus(st) && (st & MAX31329_STATUS_A2F)) {
			/* Read current time and display alarm notification */
			if (rtc.readTime()) {
				Serial.printf("ALARM2: %04d-%02d-%02d %02d:%02d:%02d\n",
					rtc.t.year, rtc.t.month, rtc.t.day,
					rtc.t.hour, rtc.t.minute, rtc.t.second);
			}
			/* Clear status register after handling alarm */
			rtc.clearStatus();
		}
	}
}
