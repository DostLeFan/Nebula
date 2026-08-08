#include <catch2/catch_test_macros.hpp>
#include <nebula/Cartridge/MBC1.hpp>
#include <sstream>
#include <vector>
#include "TestUtilities.hpp"

using namespace nebula;

TEST_CASE("MBC1 - initial state", "[mbc1]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	std::vector<uint8_t> ram(0x2000, 0x00);
	MBC1 mbc(rom, ram);
	
	// RAM disabled by default.
	REQUIRE(mbc.readRAM(0xA000) == 0xFF);
	
	// Initial ROM bank = 0x01.
	// Bank 0 is fixed (0x0000-0x3FFF).
	REQUIRE(mbc.readROM(0x0000) == 0x00); // First byte of bank 0.
	REQUIRE(mbc.readROM(0x4000) == rom[0x4000]); // Bank 1.
}

TEST_CASE("MBC1 - RAM enable with 0x0A", "[mbc1]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	std::vector<uint8_t> ram(0x2000, 0x00);
	MBC1 mbc(rom, ram);
	
	// Enable RAM.
	mbc.writeROM(0x0000, 0x0A);
	
	// Write to RAM.
	mbc.writeRAM(0xA000, 0x42);
	REQUIRE(mbc.readRAM(0xA000) == 0x42);
	
	// Disable RAM.
	mbc.writeROM(0x0000, 0x00);
	REQUIRE(mbc.readRAM(0xA000) == 0xFF);
}

TEST_CASE("MBC1 - RAM enable only accepts 0x0A", "[mbc1]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	std::vector<uint8_t> ram(0x2000, 0x00);
	MBC1 mbc(rom, ram);
	
	// Test with various values.
	mbc.writeROM(0x0000, 0x00);
	mbc.writeRAM(0xA000, 0x42);
	REQUIRE(mbc.readRAM(0xA000) == 0xFF);
	
	mbc.writeROM(0x0000, 0x0A);
	mbc.writeRAM(0xA000, 0x42);
	REQUIRE(mbc.readRAM(0xA000) == 0x42);
	
	mbc.writeROM(0x0000, 0x1A);
	mbc.writeRAM(0xA000, 0x42);
	REQUIRE(mbc.readRAM(0xA000) == 0xFF);
	
	mbc.writeROM(0x0000, 0x0B);
	mbc.writeRAM(0xA000, 0x42);
	REQUIRE(mbc.readRAM(0xA000) == 0xFF);
}

TEST_CASE("MBC1 - ROM bank switching (mode ROM)", "[mbc1]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x20000); // 128 KB (8 banks).
	std::vector<uint8_t> ram(0x2000, 0x00);
	MBC1 mbc(rom, ram);
	
	// Default mode is ROM (0x6000 = 0x00).
	
	// Select bank 2.
	mbc.writeROM(0x2000, 0x02);
	REQUIRE(mbc.readROM(0x4000) == rom[0x4000 * 2 + 0]);
	
	// Bank 0 -> converted to 1.
	mbc.writeROM(0x2000, 0x00);
	REQUIRE(mbc.readROM(0x4000) == rom[0x4000 * 1 + 0]);
}

TEST_CASE("MBC1 - ROM bank switching with upper bits (mode ROM)", "[mbc1]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x200000); // 2 MB (128 banks) -- needs the full 7 bits.
	std::vector<uint8_t> ram(0x2000, 0x00);
	MBC1 mbc(rom, ram);
	
	// Mode ROM: upper bits from 0x4000-0x5FFF are used as bits 5-6 of ROM bank.
	mbc.writeROM(0x2000, 0x01); // Lower 5 bits = 1.
	mbc.writeROM(0x4000, 0x02); // Upper 2 bits = 2 (0b10) -> (2 << 5) = 0x40 -> bank 0x41.
	REQUIRE(mbc.readROM(0x4000) == rom[0x4000 * 0x41 + 0]);
	
	// Test another combination.
	mbc.writeROM(0x2000, 0x03);
	mbc.writeROM(0x4000, 0x01); // Bits 5-6 = 1 -> bank 0x23.
	REQUIRE(mbc.readROM(0x4000) == rom[0x4000 * 0x23 + 0]);
	
	// Bank 0 with upper bits = 0 -> bank 1 (since lower bits 0 -> 1).
	mbc.writeROM(0x2000, 0x00);
	mbc.writeROM(0x4000, 0x00);
	REQUIRE(mbc.readROM(0x4000) == rom[0x4000 * 1 + 0]);
}

TEST_CASE("MBC1 - ROM bank switching ignores upper bits in RAM mode", "[mbc1]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x80000);
	std::vector<uint8_t> ram(0x2000, 0x00);
	MBC1 mbc(rom, ram);
	
	// Switch to RAM mode.
	mbc.writeROM(0x6000, 0x01);
	
	// Write upper bits.
	mbc.writeROM(0x2000, 0x01);
	mbc.writeROM(0x4000, 0x02); // Should be ignored for ROM bank.
	
	// Effective ROM bank should only use lower 5 bits = 0x01.
	REQUIRE(mbc.readROM(0x4000) == rom[0x4000 * 0x01 + 0]);
	
	// Bank 0 -> converted to 1.
	mbc.writeROM(0x2000, 0x00);
	REQUIRE(mbc.readROM(0x4000) == rom[0x4000 * 1 + 0]);
}

TEST_CASE("MBC1 - RAM bank switching (mode RAM)", "[mbc1]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	std::vector<uint8_t> ram(0x8000, 0x00); // 32 KB (4 banks).
	MBC1 mbc(rom, ram);
	
	// Enable RAM.
	mbc.writeROM(0x0000, 0x0A);
	
	// Switch to RAM mode.
	mbc.writeROM(0x6000, 0x01);
	
	// Select RAM bank 2.
	mbc.writeROM(0x4000, 0x02);
	mbc.writeRAM(0xA000, 0x42);
	REQUIRE(mbc.readRAM(0xA000) == 0x42);
	
	// Switch to RAM bank 0.
	mbc.writeROM(0x4000, 0x00);
	REQUIRE(mbc.readRAM(0xA000) == 0x00); // Bank 0, not yet written.
	
	// Write to bank 0.
	mbc.writeRAM(0xA000, 0x7F);
	REQUIRE(mbc.readRAM(0xA000) == 0x7F);
	
	// Switch back to bank 2 and check value.
	mbc.writeROM(0x4000, 0x02);
	REQUIRE(mbc.readRAM(0xA000) == 0x42);
}

TEST_CASE("MBC1 - RAM bank switching limited to 2 bits (0-3)", "[mbc1]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	std::vector<uint8_t> ram(0x8000, 0x00);
	MBC1 mbc(rom, ram);
	
	mbc.writeROM(0x0000, 0x0A);
	mbc.writeROM(0x6000, 0x01);
	
	// Write value with bits beyond 2.
	mbc.writeROM(0x4000, 0x07); // 0b0111 -> should be masked to 0x03.
	mbc.writeRAM(0xA000, 0x42);
	// Bank 3 should be selected.
	REQUIRE(mbc.readRAM(0xA000) == 0x42);
	
	// Switch to bank 0.
	mbc.writeROM(0x4000, 0x00);
	REQUIRE(mbc.readRAM(0xA000) == 0x00);
}

TEST_CASE("MBC1 - ROM bank mask based on ROM size", "[mbc1]")
{
	// 64 KB ROM (4 banks).
	std::vector<uint8_t> rom = createROMWithBankPattern(0x10000);
	std::vector<uint8_t> ram(0x2000, 0x00);
	MBC1 mbc(rom, ram);
	
	// Try to select bank 5 -> masked to 1 (only 4 banks, and bank 0 invalid).
	mbc.writeROM(0x2000, 0x05);
	REQUIRE(mbc.readROM(0x4000) == rom[0x4000 * 1 + 0]);
	
	// Select bank 3 (valid).
	mbc.writeROM(0x2000, 0x03);
	REQUIRE(mbc.readROM(0x4000) == rom[0x4000 * 3 + 0]);
	
	// With upper bits: if ROM has 4 banks, upper bits are ignored.
	mbc.writeROM(0x2000, 0x01);
	mbc.writeROM(0x4000, 0x01); // Upper bits = 1 -> bank 0x21, but masked to bank 1.
	REQUIRE(mbc.readROM(0x4000) == rom[0x4000 * 1 + 0]);
}

TEST_CASE("MBC1 - RAM size limitation", "[mbc1]")
{
	// 8 KB RAM (only bank 0).
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	std::vector<uint8_t> ram(0x2000, 0x00);
	MBC1 mbc(rom, ram);
	
	mbc.writeROM(0x0000, 0x0A);
	
	// In ROM mode, only bank 0 is accessible.
	mbc.writeRAM(0xA000, 0x42);
	REQUIRE(mbc.readRAM(0xA000) == 0x42);
	
	// Switch to RAM mode but RAM is only 8 KB, so bank switching has no effect.
	mbc.writeROM(0x6000, 0x01);
	mbc.writeROM(0x4000, 0x02); // Try to switch to bank 2.
	mbc.writeRAM(0xA000, 0x7F);
	// Bank 2 doesn't exist, so write is ignored, read returns 0xFF.
	REQUIRE(mbc.readRAM(0xA000) == 0xFF);
}

TEST_CASE("MBC1 - save/load state with single value", "[mbc1]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	std::vector<uint8_t> ram(0x8000, 0x00);
	MBC1 mbc(rom, ram);
	
	mbc.writeROM(0x0000, 0x0A);
	mbc.writeROM(0x6000, 0x01);
	mbc.writeROM(0x4000, 0x02);
	mbc.writeROM(0x2000, 0x05);
	
	// Write a value to bank 2.
	mbc.writeRAM(0xA000, 0x42);
	REQUIRE(mbc.readRAM(0xA000) == 0x42);
	
	std::ostringstream oss(std::ios::binary);
	mbc.saveState(oss);
	
	std::vector<uint8_t> rom2 = createROMWithBankPattern(0x8000);
	std::vector<uint8_t> ram2(0x8000, 0x00);
	MBC1 mbc2(rom2, ram2);
	std::istringstream iss(oss.str());
	mbc2.loadState(iss);
	
	// After loading, the MBC should be in the same state.
	// Write a new value to verify that the registers are restored.
	mbc2.writeRAM(0xA000, 0x7F);
	REQUIRE(mbc2.readRAM(0xA000) == 0x7F);
	
	// ROM bank should be restored (RAM mode, so upper bits ignored -> bank 5, masked to 1 since rom2 has only 2 banks).
	REQUIRE(mbc2.readROM(0x4000) == rom2[0x4000 * 0x01 + 0]);
}

TEST_CASE("MBC1 - save/load state with full RAM", "[mbc1]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	std::vector<uint8_t> ram(0x8000, 0x00);
	MBC1 mbc(rom, ram);
	
	mbc.writeROM(0x0000, 0x0A);
	mbc.writeROM(0x6000, 0x01);
	
	for(int bank=0;bank<4;++bank)
	{
		mbc.writeROM(0x4000, bank);
		
		for(int i=0;i<0x2000;++i)
			mbc.writeRAM(0xA000 + i, (bank * 0x2000 + i) & 0xFF);
	}
	
	std::ostringstream oss(std::ios::binary);
	mbc.saveState(oss);
	
	std::vector<uint8_t> rom2 = createROMWithBankPattern(0x8000);
	std::vector<uint8_t> ram2(0x8000, 0x00);
	MBC1 mbc2(rom2, ram2);
	std::istringstream iss(oss.str());
	mbc2.loadState(iss);
	
	// After loading, the RAM content is lost (handled by Cartridge),
	// but the MBC registers are restored. Verify by writing a new value.
	mbc2.writeROM(0x4000, 0x02); // Switch to bank 2.
	mbc2.writeRAM(0xA000, 0xAB);
	REQUIRE(mbc2.readRAM(0xA000) == 0xAB);
}

TEST_CASE("MBC1 - save/load state with disabled RAM", "[mbc1]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	std::vector<uint8_t> ram(0x2000, 0x00);
	MBC1 mbc(rom, ram);
	
	// RAM disabled.
	mbc.writeROM(0x0000, 0x00);
	mbc.writeROM(0x6000, 0x01);
	mbc.writeROM(0x4000, 0x02);
	mbc.writeROM(0x2000, 0x05);
	mbc.writeRAM(0xA000, 0x42); // Ignored.
	
	std::ostringstream oss(std::ios::binary);
	mbc.saveState(oss);
	
	std::vector<uint8_t> rom2 = createROMWithBankPattern(0x8000);
	std::vector<uint8_t> ram2(0x2000, 0x00);
	MBC1 mbc2(rom2, ram2);
	std::istringstream iss(oss.str());
	mbc2.loadState(iss);
	
	REQUIRE(mbc2.readRAM(0xA000) == 0xFF);
	REQUIRE(mbc2.readROM(0x4000) == rom2[0x4000 * 0x01 + 0]); // Bank 5 masked to 1 (rom2 has only 2 banks).
}

TEST_CASE("MBC1 - ROM size less than 32 KB", "[mbc1]")
{
	// 16 KB ROM (single bank).
	std::vector<uint8_t> rom = createROMWithBankPattern(0x4000);
	std::vector<uint8_t> ram(0x2000, 0x00);
	MBC1 mbc(rom, ram);
	
	// Fixed area is valid.
	REQUIRE(mbc.readROM(0x0000) == 0x00);
	// 0x3FFF value is 0xFF because pattern is i & 0xFF.
	REQUIRE(mbc.readROM(0x3FFF) == 0xFF);
	
	// Switchable area is beyond ROM size -> 0xFF.
	REQUIRE(mbc.readROM(0x4000) == 0xFF);
	REQUIRE(mbc.readROM(0x7FFF) == 0xFF);
}

TEST_CASE("MBC1 - RAM read returns 0xFF when disabled", "[mbc1]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	std::vector<uint8_t> ram(0x2000, 0x00);
	MBC1 mbc(rom, ram);
	
	// RAM disabled by default.
	REQUIRE(mbc.readRAM(0xA000) == 0xFF);
	
	// Even after writing, it should return 0xFF if disabled.
	mbc.writeRAM(0xA000, 0x42);
	REQUIRE(mbc.readRAM(0xA000) == 0xFF);
}

TEST_CASE("MBC1 - Mode switch toggles behavior", "[mbc1]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x80000);
	std::vector<uint8_t> ram(0x8000, 0x00);
	MBC1 mbc(rom, ram);
	
	mbc.writeROM(0x0000, 0x0A);
	
	// In ROM mode, RAM is fixed (bank 0).
	mbc.writeRAM(0xA000, 0x42);
	REQUIRE(mbc.readRAM(0xA000) == 0x42);
	
	// Switch to RAM mode.
	mbc.writeROM(0x6000, 0x01);
	
	// 0x4000-0x5FFF controls RAM bank.
	mbc.writeROM(0x4000, 0x02);
	// RAM should be on bank 2.
	mbc.writeRAM(0xA000, 0x7F);
	REQUIRE(mbc.readRAM(0xA000) == 0x7F);
	
	// Switch back to ROM mode.
	mbc.writeROM(0x6000, 0x00);
	// RAM should be fixed to bank 0 again, which still has 0x42.
	REQUIRE(mbc.readRAM(0xA000) == 0x42);
}