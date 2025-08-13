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

#include "kode_MAX31329.h"

static inline bool i2cWriteThenRead(TwoWire &w, uint8_t addr, uint8_t reg, uint8_t *buffer, size_t length) {
	w.beginTransmission(addr);
	w.write(reg);
	if (w.endTransmission(false) != 0) {
		return false;
	}
	uint8_t read = w.requestFrom((int)addr, (int)length, (int)true);
	if (read != length) {
		return false;
	}
	for (size_t i = 0; i < length; ++i) {
		buffer[i] = w.read();
	}
	return true;
}

static inline bool i2cWriteBytes(TwoWire &w, uint8_t addr, uint8_t reg, const uint8_t *buffer, size_t length) {
	w.beginTransmission(addr);
	w.write(reg);
	w.write(buffer, length);
	return (w.endTransmission() == 0);
}

uint8_t MAX31329::binToBcd(uint8_t v) { return (uint8_t)(((v / 10) << 4) | (v % 10)); }
uint8_t MAX31329::bcdToBin(uint8_t v) { return (uint8_t)(((v >> 4) * 10) + (v & 0x0F)); }

void MAX31329_Time::toTm(struct tm &tm) const {
	tm.tm_year = year - 1900;  // tm_year is years since 1900
	tm.tm_mon = month - 1;     // tm_mon is 0..11
	tm.tm_mday = day;
	tm.tm_hour = hour;
	tm.tm_min = minute;
	tm.tm_sec = second;
	tm.tm_wday = dayOfWeek;
	tm.tm_yday = 0;
	tm.tm_isdst = 0;
}

void MAX31329_Time::fromTm(const struct tm &tm) {
	year = tm.tm_year + 1900;  // convert back to full year
	month = tm.tm_mon + 1;     // convert back to 1..12
	day = tm.tm_mday;
	hour = tm.tm_hour;
	minute = tm.tm_min;
	second = tm.tm_sec;
	dayOfWeek = tm.tm_wday;
}

MAX31329::MAX31329()
	: wireBus(nullptr) {}

bool MAX31329::begin(int sdaPin, int sclPin, uint32_t frequency) {
    return begin(Wire, sdaPin, sclPin, frequency);
}

bool MAX31329::begin() {
    return begin(Wire, -1, -1, 400000U);
}

bool MAX31329::begin(uint32_t frequency) {
    return begin(Wire, -1, -1, frequency);
}

bool MAX31329::begin(TwoWire &wire, int sdaPin, int sclPin, uint32_t frequency) {
    wireBus = &wire;
    if (sdaPin >= 0 && sclPin >= 0) {
        wireBus->begin(sdaPin, sclPin, frequency);
    } else {
        wireBus->begin();
        wireBus->setClock(frequency);
    }
    uint8_t dummy;
    return readBytes(MAX31329_REG_STATUS, &dummy, 1);
}

bool MAX31329::isConnected() {
	uint8_t v;
	return readBytes(MAX31329_REG_STATUS, &v, 1);
}

bool MAX31329::readTime() {
	struct tm tm;
	if (!readTime(tm)) return false;
	this->t.fromTm(tm);
	return true;
}

bool MAX31329::writeTime() {
	struct tm tm;
	this->t.toTm(tm);
	return writeTime(tm);
}

bool MAX31329::readTime(struct tm &tm) {
	uint8_t regs[7];
	if (!readBytes(MAX31329_REG_SECONDS, regs, sizeof(regs))) return false;
	tm.tm_sec  = bcdToBin(regs[0] & 0x7F);
	tm.tm_min  = bcdToBin(regs[1] & 0x7F);
	tm.tm_hour = bcdToBin(regs[2] & 0x3F);
	tm.tm_wday = bcdToBin(regs[3] & 0x07) - 1; // 0..6
	tm.tm_mday = bcdToBin(regs[4] & 0x3F);
	uint8_t monRaw = regs[5];
	tm.tm_mon  = bcdToBin(monRaw & 0x1F) - 1; // 0..11
	uint8_t yearRaw = regs[6];
	bool century = (monRaw & 0x80) != 0;
	int year = bcdToBin(yearRaw);
	tm.tm_year = year + (century ? 200 : 100); // years since 1900
	tm.tm_yday = 0; tm.tm_isdst = 0;
	return true;
}

bool MAX31329::writeTime(const struct tm &tm) {
	uint8_t regs[7];
	regs[0] = binToBcd((uint8_t)tm.tm_sec);
	regs[1] = binToBcd((uint8_t)tm.tm_min);
	regs[2] = binToBcd((uint8_t)tm.tm_hour);
	regs[3] = binToBcd((uint8_t)(tm.tm_wday + 1)) & 0x07;
	regs[4] = binToBcd((uint8_t)tm.tm_mday);
	uint8_t mon = binToBcd((uint8_t)(tm.tm_mon + 1)) & 0x1F;
	uint8_t year;
	if (tm.tm_year >= 200) { mon |= 0x80; year = binToBcd((uint8_t)(tm.tm_year - 200)); }
	else if (tm.tm_year >= 100) { year = binToBcd((uint8_t)(tm.tm_year - 100)); }
	else { return false; }
	regs[5] = mon;
	regs[6] = year;
	return writeBytes(MAX31329_REG_SECONDS, regs, sizeof(regs));
}

bool MAX31329::writeTime(int year, int month, int day, int hour, int minute, int second, int dayOfWeek) {
	struct tm t = {};
	t.tm_year = year - 1900;  // tm_year is years since 1900
	t.tm_mon = month - 1;     // tm_mon is 0..11
	t.tm_mday = day;
	t.tm_hour = hour;
	t.tm_min = minute;
	t.tm_sec = second;
	t.tm_wday = dayOfWeek;
	return writeTime(t);
}

bool MAX31329::readTime(int &year, int &month, int &day, int &hour, int &minute, int &second, int &dayOfWeek) {
	struct tm t;
	if (!readTime(t)) return false;
	year = t.tm_year + 1900;  // convert back to full year
	month = t.tm_mon + 1;     // convert back to 1..12
	day = t.tm_mday;
	hour = t.tm_hour;
	minute = t.tm_min;
	second = t.tm_sec;
	dayOfWeek = t.tm_wday;
	return true;
}

bool MAX31329::readStatus(uint8_t &status) {
	return readBytes(MAX31329_REG_STATUS, &status, 1);
}

bool MAX31329::clearStatus() {
	uint8_t dummy;
	return readBytes(MAX31329_REG_STATUS, &dummy, 1);
}

bool MAX31329::enableInterrupts(uint8_t mask) {
	uint8_t en;
	if (!readBytes(MAX31329_REG_INT_EN, &en, 1)) return false;
	en |= mask;
	return writeBytes(MAX31329_REG_INT_EN, &en, 1);
}

bool MAX31329::disableInterrupts(uint8_t mask) {
	uint8_t en;
	if (!readBytes(MAX31329_REG_INT_EN, &en, 1)) return false;
	en &= (uint8_t)~mask;
	return writeBytes(MAX31329_REG_INT_EN, &en, 1);
}

bool MAX31329::startRTC() {
	uint8_t v;
	if (!readBytes(MAX31329_REG_CFG1, &v, 1)) return false;
	v |= MAX31329_CFG1_ENOSC;
	return writeBytes(MAX31329_REG_CFG1, &v, 1);
}

bool MAX31329::stopRTC() {
	uint8_t v;
	if (!readBytes(MAX31329_REG_CFG1, &v, 1)) return false;
	v &= (uint8_t)~MAX31329_CFG1_ENOSC;
	return writeBytes(MAX31329_REG_CFG1, &v, 1);
}

bool MAX31329::assertReset() {
	uint8_t v = MAX31329_RESET_SWRST;
	return writeBytes(MAX31329_REG_RTC_RESET, &v, 1);
}

bool MAX31329::releaseReset() {
	uint8_t v = 0;
	return writeBytes(MAX31329_REG_RTC_RESET, &v, 1);
}

bool MAX31329::clkoEnable(uint8_t freqSel) {
	uint8_t cfg2;
	if (!readBytes(MAX31329_REG_CFG2, &cfg2, 1)) return false;
	cfg2 |= MAX31329_CFG2_ENCLKO;
	cfg2 &= (uint8_t)~MAX31329_CFG2_CLKO_HZ_MASK;
	cfg2 |= (uint8_t)((freqSel & 0x03) << MAX31329_CFG2_CLKO_HZ_POS);
	return writeBytes(MAX31329_REG_CFG2, &cfg2, 1);
}

bool MAX31329::clkoDisable() {
	uint8_t cfg2;
	if (!readBytes(MAX31329_REG_CFG2, &cfg2, 1)) return false;
	cfg2 &= (uint8_t)~MAX31329_CFG2_ENCLKO;
	return writeBytes(MAX31329_REG_CFG2, &cfg2, 1);
}

bool MAX31329::clkinDisable() {
	uint8_t cfg2;
	if (!readBytes(MAX31329_REG_CFG2, &cfg2, 1)) return false;
	cfg2 &= (uint8_t)~MAX31329_CFG2_ENCLKIN;
	return writeBytes(MAX31329_REG_CFG2, &cfg2, 1);
}

bool MAX31329::timerConfigure(uint8_t initialValue, bool repeat, uint8_t freqSel) {
	uint8_t cfg;
	if (!readBytes(MAX31329_REG_TIMER_CONFIG, &cfg, 1)) return false;
	cfg &= (uint8_t)~MAX31329_TMR_TE;
	cfg |= MAX31329_TMR_TPAUSE;
	if (repeat) cfg |= MAX31329_TMR_TRPT; else cfg &= (uint8_t)~MAX31329_TMR_TRPT;
	cfg &= (uint8_t)~MAX31329_TMR_TFS_MASK;
	cfg |= (uint8_t)((freqSel & 0x03) << MAX31329_TMR_TFS_POS);
	if (!writeBytes(MAX31329_REG_TIMER_CONFIG, &cfg, 1)) return false;
	return writeBytes(MAX31329_REG_TIMER_INIT, &initialValue, 1);
}

bool MAX31329::timerStart() {
	uint8_t cfg;
	if (!readBytes(MAX31329_REG_TIMER_CONFIG, &cfg, 1)) return false;
	cfg |= MAX31329_TMR_TE;
	cfg &= (uint8_t)~MAX31329_TMR_TPAUSE;
	return writeBytes(MAX31329_REG_TIMER_CONFIG, &cfg, 1);
}

bool MAX31329::timerPause() {
	uint8_t cfg;
	if (!readBytes(MAX31329_REG_TIMER_CONFIG, &cfg, 1)) return false;
	cfg |= MAX31329_TMR_TE;
	cfg |= MAX31329_TMR_TPAUSE;
	return writeBytes(MAX31329_REG_TIMER_CONFIG, &cfg, 1);
}

bool MAX31329::timerContinue() {
	uint8_t cfg;
	if (!readBytes(MAX31329_REG_TIMER_CONFIG, &cfg, 1)) return false;
	cfg |= MAX31329_TMR_TE;
	cfg &= (uint8_t)~MAX31329_TMR_TPAUSE;
	return writeBytes(MAX31329_REG_TIMER_CONFIG, &cfg, 1);
}

bool MAX31329::timerStop() {
	uint8_t cfg;
	if (!readBytes(MAX31329_REG_TIMER_CONFIG, &cfg, 1)) return false;
	cfg &= (uint8_t)~MAX31329_TMR_TE;
	cfg |= MAX31329_TMR_TPAUSE;
	return writeBytes(MAX31329_REG_TIMER_CONFIG, &cfg, 1);
}

bool MAX31329::timerRead(uint8_t &value) {
	return readBytes(MAX31329_REG_TIMER_COUNT, &value, 1);
}

bool MAX31329::setPowerFailThreshold(uint8_t pfvt) {
	uint8_t v;
	if (!readBytes(MAX31329_REG_PWR_MGMT, &v, 1)) return false;
	v &= (uint8_t)~MAX31329_PWR_PFVT_MASK;
	v |= (uint8_t)((pfvt & 0x03) << MAX31329_PWR_PFVT_POS);
	return writeBytes(MAX31329_REG_PWR_MGMT, &v, 1);
}

bool MAX31329::selectSupply(uint8_t supply) {
	uint8_t v;
	if (!readBytes(MAX31329_REG_PWR_MGMT, &v, 1)) return false;
	switch (supply) {
		case 1: // VCC
			v |= MAX31329_PWR_DMAN_SEL; v &= (uint8_t)~MAX31329_PWR_D_VBACK_SEL; break;
		case 2: // VBACKUP
			v |= MAX31329_PWR_DMAN_SEL; v |= MAX31329_PWR_D_VBACK_SEL; break;
		case 0: default: // AUTO
			v &= (uint8_t)~MAX31329_PWR_DMAN_SEL; break;
	}
	return writeBytes(MAX31329_REG_PWR_MGMT, &v, 1);
}

bool MAX31329::trickleEnable(uint8_t path) {
	uint8_t v = 0;
	v |= MAX31329_TRK_D_TRKCHG_EN;
	v |= (uint8_t)((path & 0x0F) << MAX31329_TRK_D_TRICKLE_POS);
	return writeBytes(MAX31329_REG_TRICKLE, &v, 1);
}

bool MAX31329::trickleDisable() {
	uint8_t v;
	if (!readBytes(MAX31329_REG_TRICKLE, &v, 1)) return false;
	v &= (uint8_t)~MAX31329_TRK_D_TRKCHG_EN;
	return writeBytes(MAX31329_REG_TRICKLE, &v, 1);
}

bool MAX31329::readRam(uint8_t offset, uint8_t *buffer, size_t length) {
	if (!buffer || length == 0) return false;
	uint16_t span = (uint16_t)(MAX31329_REG_RAM_END - MAX31329_REG_RAM_START + 1);
	if ((uint16_t)offset + length > span) return false;
	return readBytes((uint8_t)(MAX31329_REG_RAM_START + offset), buffer, length);
}

bool MAX31329::writeRam(uint8_t offset, const uint8_t *buffer, size_t length) {
	if (!buffer || length == 0) return false;
	uint16_t span = (uint16_t)(MAX31329_REG_RAM_END - MAX31329_REG_RAM_START + 1);
	if ((uint16_t)offset + length > span) return false;
	return writeBytes((uint8_t)(MAX31329_REG_RAM_START + offset), buffer, length);
}

bool MAX31329::readBytes(uint8_t reg, uint8_t *buffer, size_t length) {
	if (!wireBus) return false;
	return i2cWriteThenRead(*wireBus, MAX31329_I2C_ADDRESS, reg, buffer, length);
}

bool MAX31329::writeBytes(uint8_t reg, const uint8_t *buffer, size_t length) {
	if (!wireBus) return false;
	return i2cWriteBytes(*wireBus, MAX31329_I2C_ADDRESS, reg, buffer, length);
}
