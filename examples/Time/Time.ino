#include <MAX31329.h>

MAX31329 rtc;

static void printTime()
{
	if (!rtc.readTime()) {
		Serial.println("readTime failed");
		return;
	}
	Serial.printf("%04d-%02d-%02d %02d:%02d:%02d\n", 
		rtc.t.year, rtc.t.month, rtc.t.day, 
		rtc.t.hour, rtc.t.minute, rtc.t.second);
}

void setup()
{
	Serial.begin(115200);
	Serial.println("MAX31329 Time example");
	// Init I2C on default pins; adjust if needed: rtc.begin(Wire, SDA, SCL, 400000);
	rtc.begin();

	// Set a known time (24-11-2024 15:10:00) - fluent API!
	rtc.t.year = 2024;
	rtc.t.month = 11;
	rtc.t.day = 24;
	rtc.t.hour = 15;
	rtc.t.minute = 10;
	rtc.t.second = 0;
	rtc.t.dayOfWeek = 0; // Sunday
	
	if (!rtc.writeTime()) {
		Serial.println("writeTime failed");
	}
}

void loop()
{
	delay(1000);
	printTime();
}
