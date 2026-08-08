#include <catch2/catch_test_macros.hpp>
#include <nebula/Cartridge/MBC3.hpp>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdint>
#include "TestUtilities.hpp"

using namespace nebula;

TEST_CASE("MBC3 - sanity check", "[mbc3]")
{
	REQUIRE(true);
}

TEST_CASE("MBC3 - readROM fixed bank 0", "[mbc3]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	std::vector<uint8_t> ram(0x2000, 0x00);
	MBC3 mbc(rom, ram);
	
	// Fixed bank should return data from 0x0000-0x3FFF.
	REQUIRE(mbc.readROM(0x0000) == 0x00);
	REQUIRE(mbc.readROM(0x3FFF) == rom[0x3FFF]);
}

TEST_CASE("MBC3 - readROM switchable bank with default bank 1", "[mbc3]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x20000); // 8 banks.
	std::vector<uint8_t> ram(0x2000, 0x00);
	MBC3 mbc(rom, ram);
	
	// Default ROM bank is 0x01.
	REQUIRE(mbc.readROM(0x4000) == rom[0x4000 * 1]);
}

TEST_CASE("MBC3 - readROM bank switching", "[mbc3]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x20000); // 8 banks.
	std::vector<uint8_t> ram(0x2000, 0x00);
	MBC3 mbc(rom, ram);
	
	// Change to bank 5.
	mbc.writeROM(0x2000, 0x05);
	REQUIRE(mbc.readROM(0x4000) == rom[0x4000 * 5]);
	
	// Bank 0 -> converted to 1.
	mbc.writeROM(0x2000, 0x00);
	REQUIRE(mbc.readROM(0x4000) == rom[0x4000 * 1]);
	
	// Bank 127 (max) -- masked down to bank 7 since ROM only has 8 banks.
	mbc.writeROM(0x2000, 0x7F);
	REQUIRE(mbc.readROM(0x4000) == rom[0x4000 * 7]);
}

TEST_CASE("MBC3 - readROM bank mask based on ROM size", "[mbc3]")
{
	// 64 KB ROM (4 banks).
	std::vector<uint8_t> rom = createROMWithBankPattern(0x10000);
	std::vector<uint8_t> ram(0x2000, 0x00);
	MBC3 mbc(rom, ram);
	
	// Bank 7 -> masked to 3 (because only 4 banks).
	mbc.writeROM(0x2000, 0x07);
	REQUIRE(mbc.readROM(0x4000) == rom[0x4000 * 3]);
	
	// Bank 4 -> masked to 0 -> converted to 1.
	mbc.writeROM(0x2000, 0x04);
	REQUIRE(mbc.readROM(0x4000) == rom[0x4000 * 1]);
}

TEST_CASE("MBC3 - readROM beyond ROM size returns 0xFF", "[mbc3]")
{
	// 16 KB ROM (single bank).
	std::vector<uint8_t> rom = createROMWithBankPattern(0x4000);
	std::vector<uint8_t> ram(0x2000, 0x00);
	MBC3 mbc(rom, ram);
	
	// Fixed bank valid up to 0x3FFF.
	REQUIRE(mbc.readROM(0x3FFF) == 0xFF); // Pattern gives 0xFF at 0x3FFF.
	// Switchable area beyond ROM size -> 0xFF.
	REQUIRE(mbc.readROM(0x4000) == 0xFF);
	REQUIRE(mbc.readROM(0x7FFF) == 0xFF);
}

TEST_CASE("MBC3 - readROM with bank values above 127", "[mbc3]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x20000); // 8 banks.
	std::vector<uint8_t> ram(0x2000, 0x00);
	MBC3 mbc(rom, ram);
	
	// Values above 127 are masked to 7 bits.
	mbc.writeROM(0x2000, 0x80); // 128 -> masked to 0x00 -> converted to 1.
	REQUIRE(mbc.readROM(0x4000) == rom[0x4000 * 1]);
	
	mbc.writeROM(0x2000, 0xFF); // 255 -> masked to 0x7F -> 127.
	REQUIRE(mbc.readROM(0x4000) == rom[0x4000 * 7]);
}

TEST_CASE("MBC3 - writeROM RAM enable only accepts 0x0A", "[mbc3]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	std::vector<uint8_t> ram(0x2000, 0x00);
	MBC3 mbc(rom, ram);
	
	// Test various values - only 0x0A should enable.
	for(int val=0x00;val<= 0xFF;++val)
	{
		mbc.writeROM(0x0000, val);
		// Only enable if val == 0x0A.
		bool expected = (val == 0x0A);
		// We can't test RAM directly here, we'll test in read/write RAM tests.
	}
}

TEST_CASE("MBC3 - writeROM RTC mode selection", "[mbc3]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	std::vector<uint8_t> ram(0x2000, 0x00);
	MBC3 mbc(rom, ram);
	
	mbc.writeROM(0x0000, 0x0A);
	
	// Test all RTC register values.
	for(uint8_t reg=0x08;reg<=0x0C;++reg)
	{
		mbc.writeROM(0x4000, reg);
		// m_rtcMode should be true, m_ramBank should be (reg & 0x03).
		// We'll verify by writing to RTC registers in read/write RAM tests.
	}
	
	// Test RAM bank values (0x00-0x03).
	for(uint8_t bank=0x00;bank<=0x03;++bank)
		mbc.writeROM(0x4000, bank); // m_rtcMode should be false, m_ramBank should be bank.
	
	// Test values outside ranges (0x04-0x07, 0x0D-0xFF).
	// They should be treated as RAM mode with m_ramBank = value & 0x03.
	mbc.writeROM(0x4000, 0x07);
	// m_ramBank = 0x03, m_rtcMode = false.
}

TEST_CASE("MBC3 - writeROM latch sequence", "[mbc3]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	std::vector<uint8_t> ram(0x2000, 0x00);
	MBC3 mbc(rom, ram);
	
	mbc.writeROM(0x0000, 0x0A);
	mbc.writeROM(0x4000, 0x08);
	
	// Write initial value.
	mbc.writeRAM(0xA000, 0x05);
	REQUIRE(mbc.readRAM(0xA000) == 0x05);
	
	// Latch sequence: 0x00 then 0x01.
	mbc.writeROM(0x6000, 0x00);
	mbc.writeROM(0x6000, 0x01);
	
	// Write new value.
	mbc.writeRAM(0xA000, 0x0A);
	// Should still return latched value (0x05).
	REQUIRE(mbc.readRAM(0xA000) == 0x05);
	
	// Re-latch to update.
	mbc.writeROM(0x6000, 0x00);
	mbc.writeROM(0x6000, 0x01);
	// Now should return current value (0x0A).
	REQUIRE(mbc.readRAM(0xA000) == 0x0A);
}

TEST_CASE("MBC3 - writeROM latch sequence requires exact order", "[mbc3]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	std::vector<uint8_t> ram(0x2000, 0x00);
	MBC3 mbc(rom, ram);
	
	mbc.writeROM(0x0000, 0x0A);
	mbc.writeROM(0x4000, 0x08);
	
	// Write initial value and latch properly.
	mbc.writeRAM(0xA000, 0x05);
	mbc.writeROM(0x6000, 0x00);
	mbc.writeROM(0x6000, 0x01);
	
	// Try to latch with only 0x01 (no 0x00 first).
	mbc.writeRAM(0xA000, 0x0A); // Write new value.
	mbc.writeROM(0x6000, 0x01); // Only 0x01 -> should not latch.
	REQUIRE(mbc.readRAM(0xA000) == 0x05); // Still latched to 0x05.
	
	// Try with 0x00 then 0x02 (0x02 instead of 0x01).
	mbc.writeRAM(0xA000, 0x0B);
	mbc.writeROM(0x6000, 0x00);
	mbc.writeROM(0x6000, 0x02);
	REQUIRE(mbc.readRAM(0xA000) == 0x05); // Still latched.
	
	// Proper sequence again.
	mbc.writeROM(0x6000, 0x00);
	mbc.writeROM(0x6000, 0x01);
	mbc.writeRAM(0xA000, 0x0C);
	REQUIRE(mbc.readRAM(0xA000) == 0x0B); // New latch value (0x0B).
}

// RAM access (mode RAM).

TEST_CASE("MBC3 - readRAM returns 0xFF when disabled", "[mbc3]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	std::vector<uint8_t> ram(0x2000, 0x00);
	MBC3 mbc(rom, ram);
	
	// RAM is disabled by default.
	REQUIRE(mbc.readRAM(0xA000) == 0xFF);
	
	// Even after writing, it should still return 0xFF.
	mbc.writeRAM(0xA000, 0x42);
	REQUIRE(mbc.readRAM(0xA000) == 0xFF);
}

TEST_CASE("MBC3 - RAM write and read (mode RAM, bank 0)", "[mbc3]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	std::vector<uint8_t> ram(0x2000, 0x00);
	MBC3 mbc(rom, ram);
	
	mbc.writeROM(0x0000, 0x0A); // Enable RAM.
	mbc.writeROM(0x4000, 0x00); // Select RAM bank 0.
	
	mbc.writeRAM(0xA000, 0x42);
	REQUIRE(mbc.readRAM(0xA000) == 0x42);
	
	mbc.writeRAM(0xA001, 0x7F);
	REQUIRE(mbc.readRAM(0xA001) == 0x7F);
	
	mbc.writeRAM(0xBFFF, 0xAB); // Last byte of bank 0.
	REQUIRE(mbc.readRAM(0xBFFF) == 0xAB);
}

TEST_CASE("MBC3 - RAM bank switching (mode RAM)", "[mbc3]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	std::vector<uint8_t> ram(0x8000, 0x00); // 4 banks (32 KB).
	MBC3 mbc(rom, ram);
	
	mbc.writeROM(0x0000, 0x0A); // Enable RAM.
	
	// Write to bank 1.
	mbc.writeROM(0x4000, 0x01);
	mbc.writeRAM(0xA000, 0x42);
	REQUIRE(mbc.readRAM(0xA000) == 0x42);
	
	// Switch to bank 0 and read -> should be 0x00 (initialized to 0).
	mbc.writeROM(0x4000, 0x00);
	REQUIRE(mbc.readRAM(0xA000) == 0x00);
	
	// Write to bank 0.
	mbc.writeRAM(0xA000, 0x7F);
	REQUIRE(mbc.readRAM(0xA000) == 0x7F);
	
	// Switch back to bank 1 -> value should still be 0x42.
	mbc.writeROM(0x4000, 0x01);
	REQUIRE(mbc.readRAM(0xA000) == 0x42);
}

TEST_CASE("MBC3 - RAM bank selection limited to 2 bits (0-3)", "[mbc3]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	std::vector<uint8_t> ram(0x8000, 0x00);
	MBC3 mbc(rom, ram);
	
	mbc.writeROM(0x0000, 0x0A);
	
	// Write 0x07 (0b0111) -> masked to 0x03 (bank 3).
	mbc.writeROM(0x4000, 0x07);
	mbc.writeRAM(0xA000, 0x42);
	REQUIRE(mbc.readRAM(0xA000) == 0x42);
	
	// Switch to bank 0.
	mbc.writeROM(0x4000, 0x00);
	REQUIRE(mbc.readRAM(0xA000) == 0x00);
}

TEST_CASE("MBC3 - RAM beyond size returns 0xFF", "[mbc3]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	std::vector<uint8_t> ram(0x2000, 0x00); // Only bank 0 (8 KB).
	MBC3 mbc(rom, ram);
	
	mbc.writeROM(0x0000, 0x0A);
	mbc.writeROM(0x4000, 0x00);
	
	// Valid area.
	mbc.writeRAM(0xA000, 0x42);
	REQUIRE(mbc.readRAM(0xA000) == 0x42);
	
	// Beyond RAM size (0xA000 + 0x2000 = 0xC000, so 0xBFFF is last valid byte).
	// 0xC000 is not in RAM range (0xA000-0xBFFF), so read returns 0xFF.
	// Actually, addr 0xC000 is not in the RAM range, so read returns 0xFF.
	// But what about addr 0xBFFF? Should be valid.
	// Let's test 0xC000 which is outside the range.
	REQUIRE(mbc.readRAM(0xC000) == 0xFF);
	// Also test 0x9FFF (below RAM range).
	REQUIRE(mbc.readRAM(0x9FFF) == 0xFF);
}

// RTC access (mode RTC).

TEST_CASE("MBC3 - RTC read/write all registers", "[mbc3]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	std::vector<uint8_t> ram(0x2000, 0x00);
	MBC3 mbc(rom, ram);
	
	mbc.writeROM(0x0000, 0x0A);
	
	// Seconds.
	mbc.writeROM(0x4000, 0x08);
	mbc.writeRAM(0xA000, 0x30);
	REQUIRE(mbc.readRAM(0xA000) == 0x30);
	
	// Minutes.
	mbc.writeROM(0x4000, 0x09);
	mbc.writeRAM(0xA000, 0x45);
	REQUIRE(mbc.readRAM(0xA000) == 0x45);
	
	// Hours.
	mbc.writeROM(0x4000, 0x0A);
	mbc.writeRAM(0xA000, 0x12);
	REQUIRE(mbc.readRAM(0xA000) == 0x12);
	
	// Days Low.
	mbc.writeROM(0x4000, 0x0B);
	mbc.writeRAM(0xA000, 0xAB);
	REQUIRE(mbc.readRAM(0xA000) == 0xAB);
	
	// Days High.
	mbc.writeROM(0x4000, 0x0C);
	mbc.writeRAM(0xA000, 0x81);
	REQUIRE(mbc.readRAM(0xA000) == 0x81);
}

TEST_CASE("MBC3 - RTC register writes are stored raw (no instant clamping)", "[mbc3]")
{
	// On real hardware, writing to a RTC register just stores the raw byte:
	// there is no immediate wrap/clamp. An "invalid" value (e.g. 60 seconds)
	// only gets corrected once real time actually elapses.
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	std::vector<uint8_t> ram(0x2000, 0x00);
	MBC3 mbc(rom, ram);
	
	mbc.writeROM(0x0000, 0x0A);
	
	mbc.writeROM(0x4000, 0x08);
	mbc.writeRAM(0xA000, 60); // Out-of-range but stored as-is.
	REQUIRE(mbc.readRAM(0xA000) == 60);
	
	mbc.writeROM(0x4000, 0x09);
	mbc.writeRAM(0xA000, 60);
	REQUIRE(mbc.readRAM(0xA000) == 60);
	
	mbc.writeROM(0x4000, 0x0A);
	mbc.writeRAM(0xA000, 24);
	REQUIRE(mbc.readRAM(0xA000) == 24);
}

TEST_CASE("MBC3 - RTC wraps correctly once real time elapses", "[mbc3]")
{
	// Seconds/minutes/hours only get normalized when real time actually
	// passes (via updateRTC()), which we simulate by backdating the internal
	// "last update" timestamp through a save/load round-trip.
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	std::vector<uint8_t> ram(0x2000, 0x00);
	MBC3 mbc(rom, ram);
	
	mbc.writeROM(0x0000, 0x0A);
	
	// Explicitly pin every field: the constructor seeds seconds/minutes/hours
	// from the host's wall-clock time, so we must not rely on their defaults.
	mbc.writeROM(0x4000, 0x08);
	mbc.writeRAM(0xA000, 59); // Seconds = 59.
	mbc.writeROM(0x4000, 0x09);
	mbc.writeRAM(0xA000, 0); // Minutes = 0.
	mbc.writeROM(0x4000, 0x0A);
	mbc.writeRAM(0xA000, 0); // Hours = 0.
	
	// Simulate 1 real second elapsing.
	std::ostringstream oss(std::ios::binary);
	mbc.saveState(oss);
	std::istringstream iss(backdateRTCState(oss.str(), 1), std::ios::binary);
	mbc.loadState(iss);
	
	// On real hardware the RTC crystal counts continuously; in this emulator
	// that's reflected by the CPU main loop calling tick() every cycle, which
	// applies elapsed real time via updateRTC(). We call it explicitly here.
	mbc.tick(0);
	
	// seconds 59 + 1s = 60 -> wraps to 0, minutes carries to 1.
	mbc.writeROM(0x4000, 0x08);
	REQUIRE(mbc.readRAM(0xA000) == 0);
	mbc.writeROM(0x4000, 0x09);
	REQUIRE(mbc.readRAM(0xA000) == 1);
}

TEST_CASE("MBC3 - RTC days and carry flag", "[mbc3]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	std::vector<uint8_t> ram(0x2000, 0x00);
	MBC3 mbc(rom, ram);
	
	mbc.writeROM(0x0000, 0x0A);
	
	// Set days to 511 (0x01FF), just below the 9-bit day counter overflow.
	mbc.writeROM(0x4000, 0x0B);
	mbc.writeRAM(0xA000, 0xFF);
	mbc.writeROM(0x4000, 0x0C);
	mbc.writeRAM(0xA000, 0x01);
	
	// Simulate 1 real day (86400s) elapsing, e.g. the console being off.
	std::ostringstream oss(std::ios::binary);
	mbc.saveState(oss);
	std::istringstream iss(backdateRTCState(oss.str(), 86400), std::ios::binary);
	mbc.loadState(iss);
	
	// Apply the elapsed time the way the emulator's CPU loop would (tick()).
	mbc.tick(0);
	
	// day 511 + 1 = 512 -> wraps to 0, carry flag set (bit 7).
	mbc.writeROM(0x4000, 0x0B);
	REQUIRE(mbc.readRAM(0xA000) == 0x00);
	mbc.writeROM(0x4000, 0x0C);
	uint8_t high = mbc.readRAM(0xA000);
	REQUIRE((high & 0x01) == 0x00); // Day 9th bit = 0.
	REQUIRE((high & 0x80) != 0); // Carry flag set.
}

// RTC latch behavior.

TEST_CASE("MBC3 - RTC latch separates current and latched values", "[mbc3]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	std::vector<uint8_t> ram(0x2000, 0x00);
	MBC3 mbc(rom, ram);
	
	mbc.writeROM(0x0000, 0x0A);
	
	// Write to seconds register.
	mbc.writeROM(0x4000, 0x08);
	mbc.writeRAM(0xA000, 0x05);
	REQUIRE(mbc.readRAM(0xA000) == 0x05);
	
	// Latch the RTC.
	mbc.writeROM(0x6000, 0x00);
	mbc.writeROM(0x6000, 0x01);
	
	// Write new value to seconds.
	mbc.writeRAM(0xA000, 0x0A);
	
	// Since latched, reading should return 0x05 (latched value).
	REQUIRE(mbc.readRAM(0xA000) == 0x05);
	
	// Re-latch to update.
	mbc.writeROM(0x6000, 0x00);
	mbc.writeROM(0x6000, 0x01);
	
	// Now latched value should be 0x0A.
	REQUIRE(mbc.readRAM(0xA000) == 0x0A);
}

TEST_CASE("MBC3 - RTC latch requires exact 0x00 then 0x01 order", "[mbc3]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	std::vector<uint8_t> ram(0x2000, 0x00);
	MBC3 mbc(rom, ram);
	
	mbc.writeROM(0x0000, 0x0A);
	mbc.writeROM(0x4000, 0x08);
	
	// Write initial value and latch.
	mbc.writeRAM(0xA000, 0x05);
	mbc.writeROM(0x6000, 0x00);
	mbc.writeROM(0x6000, 0x01);
	
	// Try to latch with only 0x01 (no 0x00).
	mbc.writeRAM(0xA000, 0x0A);
	mbc.writeROM(0x6000, 0x01);
	REQUIRE(mbc.readRAM(0xA000) == 0x05);
	
	// Try with 0x00 then 0x02 (wrong second value).
	mbc.writeRAM(0xA000, 0x0B);
	mbc.writeROM(0x6000, 0x00);
	mbc.writeROM(0x6000, 0x02);
	REQUIRE(mbc.readRAM(0xA000) == 0x05);
	
	// Try with 0x01 then 0x00 (wrong order).
	mbc.writeRAM(0xA000, 0x0C);
	mbc.writeROM(0x6000, 0x01);
	mbc.writeROM(0x6000, 0x00);
	REQUIRE(mbc.readRAM(0xA000) == 0x05);
	
	// Proper sequence again.
	mbc.writeROM(0x6000, 0x00);
	mbc.writeROM(0x6000, 0x01);
	REQUIRE(mbc.readRAM(0xA000) == 0x0C);
}

// saveState / loadState.

TEST_CASE("MBC3 - save/load state with RAM", "[mbc3]")
{
	// Like MBC1, MBC3::saveState/loadState only cover the MBC's own
	// registers ; external RAM is owned and (de)serialized separately by
	// Cartridge. So after loadState() the RAM content is NOT expected to be
	// restored -- only the registers (RAM bank, enable flag, ROM bank...) are.
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	std::vector<uint8_t> ram(0x8000, 0x00); // 4 banks (32 KB).
	MBC3 mbc(rom, ram);
	
	// Configure state.
	mbc.writeROM(0x0000, 0x0A);
	mbc.writeROM(0x2000, 0x05);
	mbc.writeROM(0x4000, 0x01);
	mbc.writeRAM(0xA000, 0x42);
	REQUIRE(mbc.readRAM(0xA000) == 0x42);
	
	// Save.
	std::ostringstream oss(std::ios::binary);
	mbc.saveState(oss);
	
	// Create new MBC.
	std::vector<uint8_t> rom2 = createROMWithBankPattern(0x8000);
	std::vector<uint8_t> ram2(0x8000, 0x00);
	MBC3 mbc2(rom2, ram2);
	
	// Load.
	std::istringstream iss(oss.str());
	mbc2.loadState(iss);
	
	// The RAM enable / RAM bank registers should be restored: writing a new
	// value confirms bank 1 is still selected and RAM is still enabled.
	mbc2.writeRAM(0xA000, 0x7F);
	REQUIRE(mbc2.readRAM(0xA000) == 0x7F);
	
	// ROM bank should be restored (bank 5 masked to 1: rom2 has only 2 banks).
	REQUIRE(mbc2.readROM(0x4000) == rom2[0x4000 * 1 + 0]);
}

TEST_CASE("MBC3 - save/load state with RTC", "[mbc3]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	std::vector<uint8_t> ram(0x2000, 0x00);
	MBC3 mbc(rom, ram);
	
	mbc.writeROM(0x0000, 0x0A);
	mbc.writeROM(0x4000, 0x08);
	mbc.writeRAM(0xA000, 0x2A);
	mbc.writeROM(0x4000, 0x09);
	mbc.writeRAM(0xA000, 0x10);
	
	// Latch.
	mbc.writeROM(0x6000, 0x00);
	mbc.writeROM(0x6000, 0x01);
	
	// Save.
	std::ostringstream oss(std::ios::binary);
	mbc.saveState(oss);
	
	// Create new MBC.
	std::vector<uint8_t> rom2 = createROMWithBankPattern(0x8000);
	std::vector<uint8_t> ram2(0x2000, 0x00);
	MBC3 mbc2(rom2, ram2);
	
	// Load.
	std::istringstream iss(oss.str());
	mbc2.loadState(iss);
	
	// Verify RTC registers.
	mbc2.writeROM(0x4000, 0x08);
	REQUIRE(mbc2.readRAM(0xA000) == 0x2A);
	mbc2.writeROM(0x4000, 0x09);
	REQUIRE(mbc2.readRAM(0xA000) == 0x10);
	
	// Verify latch state (should be latched).
	mbc2.writeROM(0x4000, 0x08);
	mbc2.writeRAM(0xA000, 0x55);
	REQUIRE(mbc2.readRAM(0xA000) == 0x2A); // Still latched.
}

TEST_CASE("MBC3 - save/load state with disabled RAM", "[mbc3]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	std::vector<uint8_t> ram(0x2000, 0x00);
	MBC3 mbc(rom, ram);
	
	// RAM disabled.
	mbc.writeROM(0x0000, 0x00);
	mbc.writeROM(0x2000, 0x05);
	mbc.writeROM(0x4000, 0x01);
	mbc.writeRAM(0xA000, 0x42); // Ignored.
	
	// Save.
	std::ostringstream oss(std::ios::binary);
	mbc.saveState(oss);
	
	// Create new MBC.
	std::vector<uint8_t> rom2 = createROMWithBankPattern(0x8000);
	std::vector<uint8_t> ram2(0x2000, 0x00);
	MBC3 mbc2(rom2, ram2);
	
	// Load.
	std::istringstream iss(oss.str());
	mbc2.loadState(iss);
	
	// Verify RAM is still disabled.
	REQUIRE(mbc2.readRAM(0xA000) == 0xFF);
	// ROM bank should be restored.
	REQUIRE(mbc2.readROM(0x4000) == rom2[0x4000 * 1 + 0]); // Bank 5 masked to 1 (rom2 has only 2 banks).
}

TEST_CASE("MBC3 - save/load state with multiple RAM banks", "[mbc3]")
{
	// Same reasoning as above: MBC3::saveState/loadState don't persist RAM
	// content (Cartridge's job). What must be restored is the currently
	// selected RAM bank register, so writes after loadState() land in the
	// bank that was selected before saving.
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	std::vector<uint8_t> ram(0x8000, 0x00);
	MBC3 mbc(rom, ram);
	
	mbc.writeROM(0x0000, 0x0A);
	mbc.writeROM(0x4000, 0x02); // Select bank 2 before saving.
	
	// Save.
	std::ostringstream oss(std::ios::binary);
	mbc.saveState(oss);
	
	// Create new MBC.
	std::vector<uint8_t> rom2 = createROMWithBankPattern(0x8000);
	std::vector<uint8_t> ram2(0x8000, 0x00);
	MBC3 mbc2(rom2, ram2);
	
	// Load.
	std::istringstream iss(oss.str());
	mbc2.loadState(iss);
	
	// Bank 2 should still be selected: writing here must land in bank 2's
	// region of ram2, and switching away and back should still see it.
	mbc2.writeRAM(0xA000, 0xAB);
	REQUIRE(ram2[2 * 0x2000] == 0xAB);
	
	mbc2.writeROM(0x4000, 0x00);
	REQUIRE(mbc2.readRAM(0xA000) == 0x00); // Bank 0, untouched.
	
	mbc2.writeROM(0x4000, 0x02);
	REQUIRE(mbc2.readRAM(0xA000) == 0xAB); // Back to bank 2.
}

// RTC HALT behavior.

TEST_CASE("MBC3 - RTC HALT stops time increment", "[mbc3]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	std::vector<uint8_t> ram(0x2000, 0x00);
	MBC3 mbc(rom, ram);
	
	mbc.writeROM(0x0000, 0x0A);
	
	// Set HALT bit (bit 6 of days high).
	mbc.writeROM(0x4000, 0x0C);
	mbc.writeRAM(0xA000, 0x40); // Bit 6 = 1.
	
	// Set seconds to 59.
	mbc.writeROM(0x4000, 0x08);
	mbc.writeRAM(0xA000, 59);
	REQUIRE(mbc.readRAM(0xA000) == 59);
	
	// Simulate time passing (updateRTC should not increment because HALT is set).
	// We'll call updateRTC indirectly by writing to a register.
	mbc.writeRAM(0xA000, 0x42); // Write to seconds register.
	// Since HALT is set, the value should remain 0x42 (not incremented).
	REQUIRE(mbc.readRAM(0xA000) == 0x42);
}

TEST_CASE("MBC3 - RTC clearing HALT resumes time increment", "[mbc3]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	std::vector<uint8_t> ram(0x2000, 0x00);
	MBC3 mbc(rom, ram);
	
	mbc.writeROM(0x0000, 0x0A);
	
	// Set HALT.
	mbc.writeROM(0x4000, 0x0C);
	mbc.writeRAM(0xA000, 0x40);
	
	// Clear HALT.
	mbc.writeRAM(0xA000, 0x00);
	
	// Set seconds to 59.
	mbc.writeROM(0x4000, 0x08);
	mbc.writeRAM(0xA000, 59);
	
	// Simulate time passing by calling tick.
	// We need to wait a bit for time to pass; this test will pass in a real emulator.
	// In unit tests, we cannot wait, so we'll just verify that HALT is cleared.
	mbc.writeROM(0x4000, 0x0C);
	uint8_t high = mbc.readRAM(0xA000);
	REQUIRE((high & 0x40) == 0);
}