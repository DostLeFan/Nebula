#include <catch2/catch_test_macros.hpp>
#include <nebula/Cartridge/NoMBC.hpp>
#include <sstream>
#include <vector>

using namespace nebula;

TEST_CASE("NoMBC - readROM returns correct data", "[nombc]")
{
	std::vector<uint8_t> rom = {0x01, 0x02, 0x03, 0x04, 0xFF};
	NoMBC mbc(rom);
	
	REQUIRE(mbc.readROM(0) == 0x01);
	REQUIRE(mbc.readROM(1) == 0x02);
	REQUIRE(mbc.readROM(2) == 0x03);
	REQUIRE(mbc.readROM(3) == 0x04);
	REQUIRE(mbc.readROM(4) == 0xFF);
	REQUIRE(mbc.readROM(100) == 0xFF); // Beyond the size.
}

TEST_CASE("NoMBC - writeROM does nothing", "[nombc]")
{
	std::vector<uint8_t> rom = {0x01, 0x02, 0x03};
	NoMBC mbc(rom);
	
	mbc.writeROM(0, 0xAA);
	
	REQUIRE(mbc.readROM(0) == 0x01); // Unchanged.
	
	mbc.writeROM(10, 0xBB);
	// Nothing happens.
}

TEST_CASE("NoMBC - readRAM returns 0xFF always", "[nombc]")
{
	std::vector<uint8_t> rom = {0x01};
	NoMBC mbc(rom);
	
	REQUIRE(mbc.readRAM(0) == 0xFF);
	REQUIRE(mbc.readRAM(0xA000) == 0xFF);
}

TEST_CASE("NoMBC - writeRAM ignored", "[nombc]")
{
	std::vector<uint8_t> rom = {0x01};
	NoMBC mbc(rom);
	
	mbc.writeRAM(0, 0xCC);
	
	REQUIRE(mbc.readRAM(0) == 0xFF); // Always 0xFF.
}

TEST_CASE("NoMBC - save/load state no effect", "[nombc]")
{
	std::vector<uint8_t> rom = {0x01};
	NoMBC mbc(rom);
	
	std::ostringstream oss(std::ios::binary);
	
	mbc.saveState(oss);
	
	std::istringstream iss(oss.str());
	
	mbc.loadState(iss);
	// Nothing changes; we're just checking that it doesn't crash.
}