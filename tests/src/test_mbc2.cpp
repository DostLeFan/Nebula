#include <catch2/catch_test_macros.hpp>
#include <nebula/Cartridge/MBC2.hpp>
#include <sstream>
#include <vector>
#include "TestUtilities.hpp"

using namespace nebula;

TEST_CASE("MBC2 - initial state", "[mbc2]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	MBC2 mbc(rom);
	
	// RAM disabled by default.
	REQUIRE(mbc.readRAM(0xA000) == 0xFF);
	
	// Initial ROM bank = 0x01.
	// Bank 0 is fixed (0x0000-0x3FFF).
	REQUIRE(mbc.readROM(0x0000) == 0x00); // First byte of bank 0.
	REQUIRE(mbc.readROM(0x4000) == rom[0x4000]); // Bank 1.
}

TEST_CASE("MBC2 - RAM enable with 0x0A", "[mbc2]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	MBC2 mbc(rom);
	
	// Enable RAM.
	mbc.writeROM(0x0000, 0x0A);
	
	// Write to RAM.
	mbc.writeRAM(0xA000, 0x42);
	REQUIRE(mbc.readRAM(0xA000) == 0x02); // 0x42 & 0x0F = 0x02.
	
	// Disable RAM.
	mbc.writeROM(0x0000, 0x00);
	REQUIRE(mbc.readRAM(0xA000) == 0xFF);
}

TEST_CASE("MBC2 - RAM enable with 0x1A (bit 4 set)", "[mbc2]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	MBC2 mbc(rom);
	
	// Enable RAM with bit 4 set.
	mbc.writeROM(0x0000, 0x1A);
	mbc.writeRAM(0xA000, 0x42);
	REQUIRE(mbc.readRAM(0xA000) == 0x02);
}

TEST_CASE("MBC2 - RAM enable only accepts values with bit 4 set", "[mbc2]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	MBC2 mbc(rom);
	
	// Test with various values.
	mbc.writeROM(0x0000, 0x00);
	mbc.writeRAM(0xA000, 0x42);
	REQUIRE(mbc.readRAM(0xA000) == 0xFF);
	
	mbc.writeROM(0x0000, 0x0A);
	mbc.writeRAM(0xA000, 0x42);
	REQUIRE(mbc.readRAM(0xA000) == 0x02);
	
	mbc.writeROM(0x0000, 0x0B);
	mbc.writeRAM(0xA000, 0x42);
	REQUIRE(mbc.readRAM(0xA000) == 0xFF);
}

TEST_CASE("MBC2 - ROM bank switching", "[mbc2]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x20000); // 128 KB (8 banks).
	MBC2 mbc(rom);
	
	// Select bank 5.
	mbc.writeROM(0x2100, 0x05);
	REQUIRE(mbc.readROM(0x4000) == rom[0x4000 * 5 + 0]);
	
	// Bank 0 -> converted to 1.
	mbc.writeROM(0x2100, 0x00);
	REQUIRE(mbc.readROM(0x4000) == rom[0x4000 * 1 + 0]);
}

TEST_CASE("MBC2 - ROM bank select only affects lower 4 bits", "[mbc2]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x80000); // 512 KB (32 banks).
	MBC2 mbc(rom);
	
	// Write 0x1F (5 bits) -> only lower 4 bits are used.
	mbc.writeROM(0x2100, 0x1F);
	REQUIRE(mbc.readROM(0x4000) == rom[0x4000 * 15 + 0]); // 0x0F.
	
	// Write 0x20 (0b00100000) -> 0x00 -> converted to 1.
	mbc.writeROM(0x2100, 0x20);
	REQUIRE(mbc.readROM(0x4000) == rom[0x4000 * 1 + 0]);
}

TEST_CASE("MBC2 - writes to 0x2000-0x20FF ignored (bit 8 = 0)", "[mbc2]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x20000);
	MBC2 mbc(rom);
	
	// Write to 0x2000 (bit 8 = 0) -> ignored.
	// ROM bank should not change (stays 1).
	mbc.writeROM(0x2000, 0x05);
	REQUIRE(mbc.readROM(0x4000) == rom[0x4000 * 1 + 0]);
	
	// Write to 0x2100 (bit 8 = 1) -> taken into account.
	mbc.writeROM(0x2100, 0x05);
	REQUIRE(mbc.readROM(0x4000) == rom[0x4000 * 5 + 0]);
}

TEST_CASE("MBC2 - ROM bank mask based on ROM size", "[mbc2]")
{
	// 64 KB ROM (4 banks).
	std::vector<uint8_t> rom = createROMWithBankPattern(0x10000);
	MBC2 mbc(rom);
	
	// Try to select bank 5 -> masked to 1 because only 4 banks exist.
	mbc.writeROM(0x2100, 0x05);
	REQUIRE(mbc.readROM(0x4000) == rom[0x4000 * 1 + 0]);
	
	// Select bank 3 (valid).
	mbc.writeROM(0x2100, 0x03);
	REQUIRE(mbc.readROM(0x4000) == rom[0x4000 * 3 + 0]);
}

TEST_CASE("MBC2 - RAM size limitation (256 bytes)", "[mbc2]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	MBC2 mbc(rom);
	
	mbc.writeROM(0x0000, 0x0A);
	
	// Write to the entire RAM.
	for(int i=0;i<256;++i)
		mbc.writeRAM(0xA000 + i, i & 0x0F);
	
	// Verify values.
	for(int i=0;i<256;++i)
		REQUIRE(mbc.readRAM(0xA000 + i) == (i & 0x0F));
	
	// Beyond 0xA1FF (0xA200), RAM is not accessible.
	mbc.writeRAM(0xA200, 0xAA);
	REQUIRE(mbc.readRAM(0xA200) == 0xFF);
}

TEST_CASE("MBC2 - RAM stores only lower 4 bits", "[mbc2]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	MBC2 mbc(rom);
	
	mbc.writeROM(0x0000, 0x0A);
	
	// Write values with upper bits.
	mbc.writeRAM(0xA000, 0x42); // 0x42 = 0b01000010 -> 0x02.
	mbc.writeRAM(0xA001, 0xFF); // 0xFF -> 0x0F.
	mbc.writeRAM(0xA002, 0x00);
	
	REQUIRE(mbc.readRAM(0xA000) == 0x02);
	REQUIRE(mbc.readRAM(0xA001) == 0x0F);
	REQUIRE(mbc.readRAM(0xA002) == 0x00);
}

TEST_CASE("MBC2 - RAM returns 0x0F when reading uninitialized and enabled", "[mbc2]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	MBC2 mbc(rom);
	
	// RAM is initialized to 0x00, so reading returns 0x00.
	mbc.writeROM(0x0000, 0x0A);
	REQUIRE(mbc.readRAM(0xA000) == 0x00);
	
	// Write 0x0F then read it back.
	mbc.writeRAM(0xA000, 0x0F);
	REQUIRE(mbc.readRAM(0xA000) == 0x0F);
}

TEST_CASE("MBC2 - save/load state with single value", "[mbc2]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	MBC2 mbc(rom);
	
	mbc.writeROM(0x0000, 0x0A);
	mbc.writeROM(0x2100, 0x07);
	mbc.writeRAM(0xA000, 0x42);
	
	std::ostringstream oss(std::ios::binary);
	mbc.saveState(oss);
	
	std::vector<uint8_t> rom2 = createROMWithBankPattern(0x8000);
	MBC2 mbc2(rom2);
	std::istringstream iss(oss.str());
	mbc2.loadState(iss);
	
	REQUIRE(mbc2.readRAM(0xA000) == 0x02); // 0x42 & 0x0F = 0x02.
	REQUIRE(mbc2.readROM(0x4000) == rom2[0x4000 * 1 + 0]); // Bank 7 masked to 1 (rom2 has only 2 banks).
}

TEST_CASE("MBC2 - save/load state with full RAM", "[mbc2]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	MBC2 mbc(rom);
	
	mbc.writeROM(0x0000, 0x0A);
	for(int i=0;i<256;++i)
		mbc.writeRAM(0xA000 + i, (i * 3) & 0x0F);
	
	std::ostringstream oss(std::ios::binary);
	mbc.saveState(oss);
	
	std::vector<uint8_t> rom2 = createROMWithBankPattern(0x8000);
	MBC2 mbc2(rom2);
	std::istringstream iss(oss.str());
	mbc2.loadState(iss);
	
	for(int i=0;i<256;++i)
		REQUIRE(mbc2.readRAM(0xA000 + i) == ((i * 3) & 0x0F));
}

TEST_CASE("MBC2 - save/load state with disabled RAM", "[mbc2]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	MBC2 mbc(rom);
	
	// RAM disabled.
	mbc.writeROM(0x0000, 0x00);
	mbc.writeROM(0x2100, 0x07);
	mbc.writeRAM(0xA000, 0x42); // Ignored.
	
	std::ostringstream oss(std::ios::binary);
	mbc.saveState(oss);
	
	std::vector<uint8_t> rom2 = createROMWithBankPattern(0x8000);
	MBC2 mbc2(rom2);
	std::istringstream iss(oss.str());
	mbc2.loadState(iss);
	
	// RAM remains disabled (reading returns 0xFF).
	// ROM bank is restored.
	REQUIRE(mbc2.readRAM(0xA000) == 0xFF);
	REQUIRE(mbc2.readROM(0x4000) == rom2[0x4000 * 1 + 0]); // Bank 7 masked to 1 (rom2 has only 2 banks).
}

TEST_CASE("MBC2 - ROM size less than 32 KB", "[mbc2]")
{
	// 16 KB ROM (single bank).
	std::vector<uint8_t> rom = createROMWithBankPattern(0x4000);
	MBC2 mbc(rom);
	
	// Even if we write a bank, it doesn't exist.
	mbc.writeROM(0x2100, 0x05);
	REQUIRE(mbc.readROM(0x4000) == 0xFF); // Beyond ROM size.
}

TEST_CASE("MBC2 - RAM read returns 0x00 when enabled but empty", "[mbc2]")
{
	std::vector<uint8_t> rom = createROMWithBankPattern(0x8000);
	MBC2 mbc(rom);
	
	// Enable RAM.
	mbc.writeROM(0x0000, 0x0A);
	
	// Read an unwritten area (initialized to 0x00).
	REQUIRE(mbc.readRAM(0xA000) == 0x00);
	
	// Write then read.
	mbc.writeRAM(0xA000, 0x0F);
	REQUIRE(mbc.readRAM(0xA000) == 0x0F);
}