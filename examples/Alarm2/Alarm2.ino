#include <MAX31329.h>

MAX31329 rtc;
volatile bool alarm2Flag = false;
const int pinInterrupt = 3; // update per board wiring

static void IRAM_ATTR onInterrupt()
{
	alarm2Flag = true;
}

void setup()
{
	Serial.begin(115200);
	Serial.println("MAX31329 Alarm2 example");
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

	// Enable Alarm2 interrupt
	rtc.enableInterrupts(MAX31329_INT_A2IE);

	// Program Alarm2 EVERY MINUTE (A2 has MIN/HRS/DAY_DATE): set masks to ignore all (every minute)
	uint8_t regs[3] = {0x80, 0x80, 0x80}; // A2M2, A2M3, A2M4
	rtc.writeBytes(MAX31329_REG_ALM2_MIN, regs, sizeof(regs));

	// Clear latched flags
	rtc.clearStatus();
}

void loop()
{
	if (alarm2Flag) {
		alarm2Flag = false;
		uint8_t st;
		if (rtc.readStatus(st) && (st & MAX31329_STATUS_A2F)) {
			if (rtc.readTime()) {
				Serial.printf("ALARM2: %04d-%02d-%02d %02d:%02d:%02d\n",
					rtc.t.year, rtc.t.month, rtc.t.day,
					rtc.t.hour, rtc.t.minute, rtc.t.second);
			}
			rtc.clearStatus();
		}
	}
}
