#include <catch2/catch_test_macros.hpp>
#include <nebula/Cartridge/RomHeader.hpp>
#include <vector>
#include <cstring>
#include "test_utils.hpp"

using namespace nebula;

// Construction and parsing tests.

TEST_CASE("RomHeader - construction with valid ROM", "[rom_header]")
{
	std::vector<uint8_t> rom = createROMWithHeader(0x01, 0x02, 0x03, "TESTGAME");
	RomHeader header(rom);
	
	REQUIRE(header.mbcType == MBCType::MBC1);
	REQUIRE(header.mbcRawCode == 0x01);
	REQUIRE(header.romSizeCode == 0x02);
	REQUIRE(header.ramSizeCode == 0x03);
	REQUIRE(header.entryPoint == 0x0150); // JP $0150.
	REQUIRE(header.checksumGlobal == 0);
	
	// Title.
	std::string title(header.title.data(), 8);
	REQUIRE(title == "TESTGAME");
}

TEST_CASE("RomHeader - throws if ROM too small", "[rom_header]")
{
	std::vector<uint8_t> smallRom(0x100, 0x00);
	
	REQUIRE_THROWS_AS(RomHeader(smallRom), std::runtime_error);
}

// ROM size tests.

TEST_CASE("RomHeader - getTotalROMSize covers all codes", "[rom_header]")
{
	struct TestCase
	{
		uint8_t code;
		size_t expected;
	};
	
	std::vector<TestCase> cases = {
		{0x00, 32 * 1024},
		{0x01, 64 * 1024},
		{0x02, 128 * 1024},
		{0x03, 256 * 1024},
		{0x04, 512 * 1024},
		{0x05, 1024 * 1024},
		{0x06, 2 * 1024 * 1024},
		{0x07, 4 * 1024 * 1024},
		{0x08, 8 * 1024 * 1024},
		{0x52, 1152 * 1024},
		{0x53, 1280 * 1024},
		{0x54, 1536 * 1024},
		{0xFF, 0} // Unknown.
	};
	
	for(TestCase const& tc : cases)
	{
		std::vector<uint8_t> rom = createROMWithHeader(0x00, tc.code, 0x00);
		RomHeader header(rom);
		
		REQUIRE(header.getTotalROMSize() == tc.expected);
	}
}

// RAM capacity tests.

TEST_CASE("RomHeader - getTotalRAMSize covers all codes", "[rom_header]")
{
	struct TestCase
	{
		uint8_t code;
		size_t expected;
	};
	
	std::vector<TestCase> cases = {
		{0x00, 0},
		{0x01, 2 * 1024},
		{0x02, 8 * 1024},
		{0x03, 32 * 1024},
		{0x04, 128 * 1024},
		{0x05, 64 * 1024},
		{0xFF, 0}
	};
	
	for(TestCase const& tc : cases)
	{
		std::vector<uint8_t> rom = createROMWithHeader(0x00, 0x00, tc.code);
		RomHeader header(rom);
		
		REQUIRE(header.getTotalRAMSize() == tc.expected);
	}
}

// Test hasBattery.

TEST_CASE("RomHeader - hasBattery", "[rom_header]")
{
	// Codes with battery.
	std::vector<uint8_t> batteryCodes = {
		0x03, 0x06, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x1B, 0x1C, 0x1D, 0x1E
	};
	
	for(uint8_t code : batteryCodes)
	{
		std::vector<uint8_t> rom = createROMWithHeader(code, 0x00, 0x00);
		RomHeader header(rom);
		
		REQUIRE(header.hasBattery() == true);
	}
	
	// Codes without battery.
	std::vector<uint8_t> noBatteryCodes = {
		0x00, 0x01, 0x02, 0x05, 0x19, 0x1A
	};
	
	for(uint8_t code : noBatteryCodes)
	{
		std::vector<uint8_t> rom = createROMWithHeader(code, 0x00, 0x00);
		RomHeader header(rom);
		
		REQUIRE(header.hasBattery() == false);
	}
}

// Test verifyLogo.

TEST_CASE("RomHeader - verifyLogo", "[rom_header]")
{
	// Non-empty logo (by default).
	std::vector<uint8_t> rom = createROMWithHeader(0x00, 0x00, 0x00);
	RomHeader header(rom);
	
	REQUIRE(header.verifyLogo() == true);
	
	// Really null logo.
	std::vector<uint8_t> romNoLogo(0x150, 0x00);
	
	// We'll still include a valid entry point to avoid the exception.
	romNoLogo[0x0100] = 0xC3;
	romNoLogo[0x0101] = 0x50;
	romNoLogo[0x0102] = 0x01;
	
	// The logo remains at 0, so verifyLogo must return false.
	// But the checksum will be incorrect, so we cannot test it directly..
	// We're just going to test how the logo is parsed.
	RomHeader header2(romNoLogo);
	
	REQUIRE(header2.verifyLogo() == false);
}

// Test isValid (checksum and entry point).

TEST_CASE("RomHeader - isValid with valid header", "[rom_header]")
{
	std::vector<uint8_t> rom = createROMWithHeader(0x01, 0x02, 0x03, "TESTGAME", true);
	RomHeader header(rom);
	
	REQUIRE(header.isValid() == true);
}

TEST_CASE("RomHeader - isValid fails with invalid checksum", "[rom_header]")
{
	std::vector<uint8_t> rom = createROMWithHeader(0x01, 0x02, 0x03, "TESTGAME", false);
	RomHeader header(rom);
	
	REQUIRE(header.isValid() == false);
}

TEST_CASE("RomHeader - isValid fails with bad entry point", "[rom_header]")
{
	std::vector<uint8_t> rom = createROMWithHeader(0x01, 0x02, 0x03, "TESTGAME", true);
	
	// Corrupt the entry point.
	rom[0x0100] = 0x00;
	rom[0x0101] = 0x00;
	rom[0x0102] = 0x00;
	
	RomHeader header(rom);
	
	REQUIRE(header.isValid() == false);
}

TEST_CASE("RomHeader - isValid fails with empty logo", "[rom_header]")
{
	std::vector<uint8_t> rom(0x150, 0x00);
	
	// Valid entry point.
	rom[0x0100] = 0xC3;
	rom[0x0101] = 0x50;
	rom[0x0102] = 0x01;
	
	// We add a title so that the checksum can be calculated, but the logo is blank.
	for(int i=0;i<16;++i)
		rom[0x0134 + i] = 'A' + i;
	
	// Valid checksum.
	uint8_t sum = 0;
	
	for(uint16_t addr=0x0134;addr<=0x014C;++addr)
		sum += rom[addr];
	
	sum += 0x19;
	rom[0x014D] = static_cast<uint8_t>(-sum);
	
	// The logo is empty, so isValid must fail.
	RomHeader header(rom);
	
	REQUIRE(header.isValid() == false);
}

// Test for extracting the entry point.

TEST_CASE("RomHeader - entry point extraction", "[rom_header]")
{
	std::vector<uint8_t> rom = createROMWithHeader(0x00, 0x00, 0x00);
	RomHeader header(rom);
	
	REQUIRE(header.entryPoint == 0x0150);
}

// Test of MBC types.

TEST_CASE("RomHeader - MBC type detection", "[rom_header]")
{
	struct TestCase
	{
		uint8_t code;
		MBCType expected;
	};
	
	std::vector<TestCase> cases = {
		{0x00, MBCType::NONE},
		{0x01, MBCType::MBC1},
		{0x02, MBCType::MBC1},
		{0x03, MBCType::MBC1},
		{0x05, MBCType::MBC2},
		{0x06, MBCType::MBC2},
		{0x0F, MBCType::MBC3},
		{0x10, MBCType::MBC3},
		{0x11, MBCType::MBC3},
		{0x12, MBCType::MBC3},
		{0x13, MBCType::MBC3},
		{0x19, MBCType::MBC5},
		{0x1A, MBCType::MBC5},
		{0x1B, MBCType::MBC5},
		{0x1C, MBCType::MBC5},
		{0x1D, MBCType::MBC5},
		{0x1E, MBCType::MBC5},
		{0xFF, MBCType::NONE}
	};
	
	for(TestCase const& tc : cases)
	{
		std::vector<uint8_t> rom = createROMWithHeader(tc.code, 0x00, 0x00);
		RomHeader header(rom);
		
		REQUIRE(header.mbcType == tc.expected);
	}
}