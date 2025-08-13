#include <MAX31329.h>

MAX31329 rtc;
volatile bool alarmFlag = false;
const int pinInterrupt = 2; // update per board wiring

static void IRAM_ATTR onInterrupt()
{
	alarmFlag = true;
}

void setup()
{
	Serial.begin(115200);
	Serial.println("MAX31329 Alarm1 example");
	pinMode(pinInterrupt, INPUT);
	attachInterrupt(digitalPinToInterrupt(pinInterrupt), onInterrupt, FALLING);

	rtc.begin();

	// Set current time using fluent API
	rtc.t.year = 2024;
	rtc.t.month = 11;
	rtc.t.day = 24;
	rtc.t.hour = 15;
	rtc.t.minute = 10;
	rtc.t.second = 0;
	rtc.t.dayOfWeek = 0; // Sunday
	rtc.writeTime();

	// Enable Alarm1 interrupt
	rtc.enableInterrupts(MAX31329_INT_A1IE);

	// Program Alarm1 to trigger every minute (sec don't care)
	// Build registers directly via low-level API
	uint8_t regs[6] = {0};
	// Set all mask bits to 1 for "every minute" trigger
	regs[0] = 0x80; // A1M1 (seconds mask)
	regs[1] = 0x80; // A1M2 (minutes mask)
	regs[2] = 0x80; // A1M3 (hours mask)
	regs[3] = 0x80; // A1M4 (day/date mask)
	// Write alarm1 block: SEC..DAY_DATE
	rtc.writeBytes(MAX31329_REG_ALM1_SEC, regs, 4);
	
	// Set month and year masks
	uint8_t mon = 0x80; // A1M5 (month mask)
	uint8_t year = 0x00; // year value with A1M6 handled by mon register
	rtc.writeBytes(MAX31329_REG_ALM1_MON, &mon, 1);
	rtc.writeBytes(MAX31329_REG_ALM1_YEAR, &year, 1);

	// Clear latched flags
	rtc.clearStatus();
}

void loop()
{
	if (alarmFlag) {
		alarmFlag = false;
		uint8_t st;
		if (rtc.readStatus(st) && (st & MAX31329_STATUS_A1F)) {
			// Read current time using fluent API
			if (rtc.readTime()) {
				Serial.printf("ALARM1: %04d-%02d-%02d %02d:%02d:%02d\n",
					rtc.t.year, rtc.t.month, rtc.t.day,
					rtc.t.hour, rtc.t.minute, rtc.t.second);
			}
			rtc.clearStatus();
		}
	}
}