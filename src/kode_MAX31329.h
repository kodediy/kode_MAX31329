/*
 * Copyright 2025 KODE DIY, SOCIEDAD LIMITADA
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef KODE_MAX31329_H
#define KODE_MAX31329_H

#if !defined(ARDUINO_ARCH_ESP32)
#error "kode_MAX31329: This library targets kode dot (ESP32-S3) only."
#endif

#include <Arduino.h>
#include <Wire.h>
#include <time.h>

#include "MAX31329_registers.h"

// Time structure with convenient access
struct MAX31329_Time {
	int year = 2024;      // Full year (2024, 2025, etc.)
	int month = 1;        // 1..12
	int day = 1;          // 1..31
	int hour = 0;         // 0..23
	int minute = 0;       // 0..59
	int second = 0;       // 0..59
	int dayOfWeek = 0;    // 0..6 (0=Sunday)
	
	// Convert to/from struct tm
	void toTm(struct tm &t) const;
	void fromTm(const struct tm &t);
};

class MAX31329 {
public:
	MAX31329();

	// Direct time access - read/write updates this
	MAX31329_Time t;

	// Initialize with default Wire and custom pins (most common case)
	bool begin(int sdaPin, int sclPin, uint32_t frequency = 400000U);
	
	// Initialize with default Wire and default pins
	bool begin();
	
	// Initialize with default Wire and custom frequency
	bool begin(uint32_t frequency);
	
	// Initialize with custom TwoWire bus and optional custom pins/clock
	bool begin(TwoWire &wire,
		int sdaPin = -1,
		int sclPin = -1,
		uint32_t frequency = 400000U);

	// Connectivity check (simple read of STATUS)
	bool isConnected();

	// Primary time interface - reads/writes rtc.t
	bool readTime();  // Updates rtc.t from RTC
	bool writeTime(); // Writes rtc.t to RTC
	
	// Low-level time interface (struct tm uses: tm_year since 1900, tm_mon 0..11)
	bool readTime(struct tm &tm);
	bool writeTime(const struct tm &tm);
	
	// Convenience overloads for time operations (year is full year like 2024)
	bool writeTime(int year, int month, int day, int hour, int minute, int second, int dayOfWeek = 0);
	bool readTime(int &year, int &month, int &day, int &hour, int &minute, int &second, int &dayOfWeek);

	// Status / interrupts
	bool readStatus(uint8_t &status);
	bool clearStatus(); // read STATUS to clear latched flags
	bool enableInterrupts(uint8_t mask);
	bool disableInterrupts(uint8_t mask);

	// RTC oscillator control
	bool startRTC();
	bool stopRTC();

	// Software reset pin control register
	bool assertReset();
	bool releaseReset();

	// CLKO/CLKIN control
	// freqSel: 0..3 per datasheet (1Hz, 50Hz, 60Hz, 32kHz)
	bool clkoEnable(uint8_t freqSel);
	bool clkoDisable();
	bool clkinDisable();

	// Timer utilities
	// freqSel: 0..3 per datasheet (1024/256/64/16 Hz)
	bool timerConfigure(uint8_t initialValue, bool repeat, uint8_t freqSel);
	bool timerStart();
	bool timerPause();
	bool timerContinue();
	bool timerStop();
	bool timerRead(uint8_t &value);

	// Power management and trickle charger
	// pfvt: 0..3 threshold per datasheet
	bool setPowerFailThreshold(uint8_t pfvt);
	// supply: 0 auto, 1 vcc, 2 vbackup
	bool selectSupply(uint8_t supply);
	// Trickle: path encoded 0..0xF per datasheet table
	bool trickleEnable(uint8_t path);
	bool trickleDisable();

	// NVRAM access
	bool readRam(uint8_t offset, uint8_t *buffer, size_t length);
	bool writeRam(uint8_t offset, const uint8_t *buffer, size_t length);

	// Low-level register access
	bool readBytes(uint8_t reg, uint8_t *buffer, size_t length);
	bool writeBytes(uint8_t reg, const uint8_t *buffer, size_t length);

private:
	TwoWire *wireBus;

	static uint8_t binToBcd(uint8_t v);
	static uint8_t bcdToBin(uint8_t v);
};

#endif // KODE_MAX31329_H
