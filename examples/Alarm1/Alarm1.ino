/**
 * MAX31329 Alarm1 interrupt demo: configures Alarm1 to trigger every minute using hardware interrupts.
 * Uses low-level register access to set alarm masks and GPIO interrupt handling on ESP32-S3.
 * Prints timestamped alarm notifications when interrupt occurs.
 */
/* ───────── KODE | docs.kode.diy ───────── */

#include <MAX31329.h>

MAX31329 rtc;
volatile bool alarmFlag = false;
const int pinInterrupt = 2; /* GPIO pin connected to MAX31329 INT output - adjust for your board */

/* Interrupt Service Routine - must be in IRAM for ESP32-S3 */
static void IRAM_ATTR onInterrupt()
{
	alarmFlag = true;
}

void setup()
{
	Serial.begin(115200);
	Serial.println("MAX31329 Alarm1 example");
	
	/* Configure interrupt pin and attach ISR */
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

	/* Enable Alarm1 interrupt in RTC */
	rtc.enableInterrupts(MAX31329_INT_A1IE);

	/* Configure Alarm1 to trigger every minute using register masks */
	uint8_t regs[6] = {0};
	/* Set all mask bits to 1 for "every minute" trigger - when mask=1, field is ignored */
	regs[0] = 0x80; /* A1M1 (seconds mask) - ignore seconds */
	regs[1] = 0x80; /* A1M2 (minutes mask) - ignore minutes */
	regs[2] = 0x80; /* A1M3 (hours mask) - ignore hours */
	regs[3] = 0x80; /* A1M4 (day/date mask) - ignore day/date */
	
	/* Write alarm1 registers: SEC, MIN, HRS, DAY_DATE */
	rtc.writeBytes(MAX31329_REG_ALM1_SEC, regs, 4);
	
	/* Configure month and year alarm registers */
	uint8_t mon = 0x80;  /* A1M5 (month mask) - ignore month */
	uint8_t year = 0x00; /* Year value (A1M6 handled separately) */
	rtc.writeBytes(MAX31329_REG_ALM1_MON, &mon, 1);
	rtc.writeBytes(MAX31329_REG_ALM1_YEAR, &year, 1);

	/* Clear any pending alarm flags */
	rtc.clearStatus();
}

void loop()
{
	/* Check if alarm interrupt occurred */
	if (alarmFlag) {
		alarmFlag = false;  /* Reset flag */
		
		uint8_t st;
		/* Verify Alarm1 flag is set in status register */
		if (rtc.readStatus(st) && (st & MAX31329_STATUS_A1F)) {
			/* Read and display current time when alarm triggers */
			if (rtc.readTime()) {
				Serial.printf("ALARM1: %04d-%02d-%02d %02d:%02d:%02d\n",
					rtc.t.year, rtc.t.month, rtc.t.day,
					rtc.t.hour, rtc.t.minute, rtc.t.second);
			}
			/* Clear status flags after handling */
			rtc.clearStatus();
		}
	}
}