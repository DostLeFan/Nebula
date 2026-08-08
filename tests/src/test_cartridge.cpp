#include <catch2/catch_test_macros.hpp>
#include <nebula/Cartridge/Cartridge.hpp>
#include <nebula/Cartridge/NoMBC.hpp>
#include <sstream>
#include <vector>
#include <cstring>
#include "test_utils.hpp"

using namespace nebula;

// Tests.

TEST_CASE("Cartridge - construction with NoMBC ROM", "[cartridge]")
{
	std::vector<uint8_t> rom = createROMWithHeader(0x00, 0x00, 0x00);
	Cartridge cart(rom);
	
	REQUIRE(cart.getHeader().mbcType == MBCType::NONE);
	REQUIRE(cart.hasBattery() == false);
	
	// Check that the ROM is being read correctly.
	REQUIRE(cart.read(0x0100) == 0xC3); // Entry point.
	REQUIRE(cart.read(0x0101) == 0x50);
	REQUIRE(cart.read(0x0102) == 0x01);
	REQUIRE(cart.read(0x0134) == 'T'); // Title.
}

TEST_CASE("Cartridge - readROM with NoMBC", "[cartridge]")
{
	std::vector<uint8_t> rom = createFullROM(0x00, 0x00, 0x00, 0x4000);
	Cartridge cart(rom);
	
	// Read some values from the ROM.
	REQUIRE(cart.read(0x0100) == 0xC3);
	REQUIRE(cart.read(0x0150) == 0x50); // 0x0150 = 0x50 (pattern i & 0xFF).
	REQUIRE(cart.read(0x01FF) == 0xFF);
	REQUIRE(cart.read(0x0200) == 0x00);
	REQUIRE(cart.read(0x3FFF) == 0xFF);
}

TEST_CASE("Cartridge - writeROM ignored (read-only)", "[cartridge]")
{
	std::vector<uint8_t> rom = createROMWithHeader(0x00, 0x00, 0x00);
	Cartridge cart(rom);
	
	uint8_t original = cart.read(0x0000);
	cart.write(0x0000, 0xAA);
	REQUIRE(cart.read(0x0000) == original); // Unchanged.
}

TEST_CASE("Cartridge - readRAM returns 0xFF when no RAM", "[cartridge]")
{
	std::vector<uint8_t> rom = createROMWithHeader(0x00, 0x00, 0x00);
	Cartridge cart(rom);
	
	REQUIRE(cart.read(0xA000) == 0xFF);
	REQUIRE(cart.read(0xBFFF) == 0xFF);
}

TEST_CASE("Cartridge - writeRAM ignored when no RAM", "[cartridge]")
{
	std::vector<uint8_t> rom = createROMWithHeader(0x00, 0x00, 0x00);
	Cartridge cart(rom);
	
	cart.write(0xA000, 0x42);
	REQUIRE(cart.read(0xA000) == 0xFF); // Always 0xFF.
}

TEST_CASE("Cartridge - hasBattery reflects header", "[cartridge]")
{
	// Without battery.
	std::vector<uint8_t> rom1 = createROMWithHeader(0x01, 0x00, 0x00);
	Cartridge cart1(rom1);
	REQUIRE(cart1.hasBattery() == false);
	
	// With battery (MBC1+RAM+Battery).
	std::vector<uint8_t> rom2 = createROMWithHeader(0x03, 0x00, 0x00);
	Cartridge cart2(rom2);
	REQUIRE(cart2.hasBattery() == true);
}

TEST_CASE("Cartridge - save/load state", "[cartridge]")
{
	std::vector<uint8_t> rom = createFullROM(0x00, 0x00, 0x00, 0x4000);
	Cartridge cart(rom);
	
	// Modify the RAM (even if it doesn't exist, it doesn't matter)
	// For this test, we're using a ROM with RAM to test the save function
	// We're going to use a ROM with MBC1+RAM (but we don't have MBC1 yet,
	// so we'll simulate it by using NoMBC and forcing the RAM manually.
	// In reality, this test will be more useful once MBC1 has been implemented.
	// For now, we're just testing that save/load doesn't cause a crash.
	
	std::ostringstream oss(std::ios::binary);
	cart.saveState(oss);
	
	std::istringstream iss(oss.str());
	cart.loadState(iss);
	
	// Nothing should have changed.
	REQUIRE(cart.read(0x0100) == 0xC3);
}

TEST_CASE("Cartridge - RAM size allocation", "[cartridge]")
{
	// ROM with 8 KB of RAM (code 0x02).
	std::vector<uint8_t> rom = createROMWithHeader(0x03, 0x00, 0x02);
	Cartridge cart(rom);
	
	// We cannot test the RAM directly as we do not yet have MBC1,
	// but we can check that the constructor doesn't crash.
	REQUIRE(cart.getHeader().getTotalRAMSize() == 8 * 1024);
}

TEST_CASE("Cartridge - invalid MBC type falls back to NoMBC", "[cartridge]")
{
	std::vector<uint8_t> rom = createROMWithHeader(0xFF, 0x00, 0x00);
	Cartridge cart(rom);
	
	// The MBC type should be NONE (fallback).
	REQUIRE(cart.getHeader().mbcType == MBCType::NONE);
	REQUIRE(cart.read(0x0100) == 0xC3); // Works with NoMBC.
}