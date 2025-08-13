#include <MAX31329.h>

MAX31329 rtc;
volatile bool timerFlag = false;
const int pinInterrupt = 2; // update per board wiring

static void IRAM_ATTR onInterrupt()
{
	timerFlag = true;
}

void setup()
{
	Serial.begin(115200);
	Serial.println("MAX31329 Timer example");
	pinMode(pinInterrupt, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(pinInterrupt), onInterrupt, FALLING);

	rtc.begin();

	// Configure RTC time using fluent API
	rtc.t.year = 2024;
	rtc.t.month = 11;
	rtc.t.day = 24;
	rtc.t.hour = 15;
	rtc.t.minute = 10;
	rtc.t.second = 0;
	rtc.t.dayOfWeek = 0; // Sunday
	rtc.writeTime();

	// Configure timer: initial value 16, repeat true, 16Hz
	if (!rtc.timerConfigure(16, true, 3)) {
		Serial.println("timerConfigure failed");
	}
	// Enable timer interrupt (TIE)
	rtc.enableInterrupts(MAX31329_INT_TIE);

	// Start timer
	rtc.timerStart();
}

void loop()
{
	if (timerFlag) {
		timerFlag = false;
		uint8_t st;
		if (rtc.readStatus(st) && (st & MAX31329_STATUS_TIF)) {
			if (rtc.readTime()) {
				Serial.printf("TIMER: %04d-%02d-%02d %02d:%02d:%02d\n",
					rtc.t.year, rtc.t.month, rtc.t.day,
					rtc.t.hour, rtc.t.minute, rtc.t.second);
			}
			rtc.clearStatus();
		}
	}
}
