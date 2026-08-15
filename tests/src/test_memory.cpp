#include <catch2/catch_test_macros.hpp>
#include <nebula/Cartridge/Cartridge.hpp>
#include <nebula/Memory/Memory.hpp>
#include <vector>
#include <cstring>
#include "test_utils.hpp"

using namespace nebula;

static Cartridge makeNoMBCCartridge()
{
	// Small NoMBC ROM (32 KiB) with a recognisable pattern.
	std::vector<uint8_t> rom = createFullROM(0x00, 0x00, 0x00, 0x8000);
	
	return Cartridge(rom);
}

static Cartridge makeMBC1WithRAM()
{
	// MBC1 + RAM (so external RAM is usable).
	// ROM size code 0x00 = 32 KiB, RAM size code 0x02 = 8 KiB.
	std::vector<uint8_t> rom = createFullROM(0x02, 0x00, 0x02, 0x8000);
	
	return Cartridge(rom);
}

TEST_CASE("Memory - construction and power-up I/O values", "[memory]")
{
	Scheduler sched;
	Cartridge cart = makeNoMBCCartridge();
	Memory mem(cart, sched);
	
	// Key power-up values (DMG, from Pan Docs).
	REQUIRE(mem.read(0xFF00) == 0xCF); // P1.
	REQUIRE(mem.read(0xFF01) == 0x00); // SB.
	REQUIRE(mem.read(0xFF02) == 0x7E); // SC.
	REQUIRE(mem.read(0xFF04) == 0xAB); // DIV.
	REQUIRE(mem.read(0xFF05) == 0x00); // TIMA.
	REQUIRE(mem.read(0xFF06) == 0x00); // TMA.
	REQUIRE(mem.read(0xFF07) == 0xF8); // TAC.
	REQUIRE(mem.read(0xFF0F) == 0xE1); // IF.
	
	REQUIRE(mem.read(0xFF10) == 0x80); // NR10.
	REQUIRE(mem.read(0xFF11) == 0xBF); // NR11.
	REQUIRE(mem.read(0xFF12) == 0xF3); // NR12.
	REQUIRE(mem.read(0xFF24) == 0x77); // NR50.
	REQUIRE(mem.read(0xFF25) == 0xF3); // NR51.
	REQUIRE(mem.read(0xFF26) == 0xF1); // NR52.
	
	REQUIRE(mem.read(0xFF40) == 0x91); // LCDC.
	REQUIRE(mem.read(0xFF41) == 0x85); // STAT.
	REQUIRE(mem.read(0xFF42) == 0x00); // SCY.
	REQUIRE(mem.read(0xFF43) == 0x00); // SCX.
	REQUIRE(mem.read(0xFF44) == 0x00); // LY.
	REQUIRE(mem.read(0xFF45) == 0x00); // LYC.
	REQUIRE(mem.read(0xFF46) == 0xFF); // DMA.
	REQUIRE(mem.read(0xFF47) == 0xFC); // BGP.
	REQUIRE(mem.read(0xFF48) == 0xFF); // OBP0.
	REQUIRE(mem.read(0xFF49) == 0xFF); // OBP1.
	REQUIRE(mem.read(0xFF4A) == 0x00); // WY.
	REQUIRE(mem.read(0xFF4B) == 0x00); // WX.
	
	// IE starts at 0.
	REQUIRE(mem.read(0xFFFF) == 0x00);
}

TEST_CASE("Memory - ROM read (0x0000-0x7FFF)", "[memory]")
{
	Scheduler sched;
	Cartridge cart = makeNoMBCCartridge();
	Memory mem(cart, sched);
	
	REQUIRE(mem.read(0x0100) == 0xC3); // Entry point from helper.
	REQUIRE(mem.read(0x0150) == 0x50); // Pattern.
	REQUIRE(mem.read(0x3FFF) == 0xFF);
	REQUIRE(mem.read(0x4000) == 0x00); // Start of second bank (pattern).
}

TEST_CASE("Memory - ROM write is intercepted by MBC", "[memory]")
{
	Scheduler sched;
	Cartridge cart = makeNoMBCCartridge();
	Memory mem(cart, sched);
	
	uint8_t original = mem.read(0x0000);
	mem.write(0x0000, 0xAA);
	
	// NoMBC ignores writes to ROM -> value unchanged.
	REQUIRE(mem.read(0x0000) == original);
}

TEST_CASE("Memory - VRAM read/write when accessible", "[memory]")
{
	Scheduler sched;
	Cartridge cart = makeNoMBCCartridge();
	Memory mem(cart, sched);
	
	mem.setVRAMAccessible(true);
	
	mem.write(0x8000, 0x12);
	mem.write(0x9FFF, 0x34);
	
	REQUIRE(mem.read(0x8000) == 0x12);
	REQUIRE(mem.read(0x9FFF) == 0x34);
	
	// Direct PPU access also works.
	REQUIRE(mem.readVRAM(0x8000) == 0x12);
	REQUIRE(mem.readVRAM(0x9FFF) == 0x34);
}

TEST_CASE("Memory - VRAM blocked returns 0xFF and ignores writes", "[memory]")
{
	Scheduler sched;
	Cartridge cart = makeNoMBCCartridge();
	Memory mem(cart, sched);
	
	mem.setVRAMAccessible(true);
	mem.write(0x8000, 0x55);
	mem.setVRAMAccessible(false);
	
	REQUIRE(mem.read(0x8000) == 0xFF); // CPU sees 0xFF.
	REQUIRE(mem.readVRAM(0x8000) == 0x55); // PPU still sees real value.
	
	mem.write(0x8000, 0xAA); // Write ignored.
	mem.setVRAMAccessible(true);
	REQUIRE(mem.read(0x8000) == 0x55); // Unchanged.
}

TEST_CASE("Memory - External RAM when present", "[memory]")
{
	Scheduler sched;
	Cartridge cart = makeMBC1WithRAM();
	Memory mem(cart, sched);
	
	// MBC1 needs RAM enabled (write 0x0A to 0x0000-0x1FFF).
	mem.write(0x0000, 0x0A);
	
	mem.write(0xA000, 0x42);
	mem.write(0xBFFF, 0x99);
	
	REQUIRE(mem.read(0xA000) == 0x42);
	REQUIRE(mem.read(0xBFFF) == 0x99);
}

TEST_CASE("Memory - External RAM returns 0xFF when disabled / absent", "[memory]")
{
	Scheduler sched;
	Cartridge cart = makeNoMBCCartridge(); // No RAM.
	Memory mem(cart, sched);
	
	REQUIRE(mem.read(0xA000) == 0xFF);
	REQUIRE(mem.read(0xBFFF) == 0xFF);
	
	mem.write(0xA000, 0x11);
	REQUIRE(mem.read(0xA000) == 0xFF); // Still 0xFF.
}

TEST_CASE("Memory - WRAM bank 0 and bank 1", "[memory]")
{
	Scheduler sched;
	Cartridge cart = makeNoMBCCartridge();
	Memory mem(cart, sched);
	
	mem.write(0xC000, 0x11);
	mem.write(0xCFFF, 0x22);
	mem.write(0xD000, 0x33);
	mem.write(0xDFFF, 0x44);
	
	REQUIRE(mem.read(0xC000) == 0x11);
	REQUIRE(mem.read(0xCFFF) == 0x22);
	REQUIRE(mem.read(0xD000) == 0x33);
	REQUIRE(mem.read(0xDFFF) == 0x44);
}

TEST_CASE("Memory - Echo RAM mirrors WRAM", "[memory]")
{
	Scheduler sched;
	Cartridge cart = makeNoMBCCartridge();
	Memory mem(cart, sched);
	
	// Write via normal WRAM -> appears in Echo.
	mem.write(0xC000, 0xAA);
	mem.write(0xD000, 0xBB);
	REQUIRE(mem.read(0xE000) == 0xAA);
	REQUIRE(mem.read(0xF000) == 0xBB);
	
	// Write via Echo -> appears in WRAM.
	mem.write(0xE123, 0xCC);
	mem.write(0xF456, 0xDD);
	REQUIRE(mem.read(0xC123) == 0xCC);
	REQUIRE(mem.read(0xD456) == 0xDD);
	
	// Last mirrored byte (0xDDFF <-> 0xFDFF).
	mem.write(0xDDFF, 0xEE);
	REQUIRE(mem.read(0xFDFF) == 0xEE);
	mem.write(0xFDFF, 0xFF);
	REQUIRE(mem.read(0xDDFF) == 0xFF);
}

TEST_CASE("Memory - OAM read/write when accessible", "[memory]")
{
	Scheduler sched;
	Cartridge cart = makeNoMBCCartridge();
	Memory mem(cart, sched);
	
	mem.setOAMAccessible(true);
	
	mem.write(0xFE00, 0x10);
	mem.write(0xFE9F, 0x20);
	
	REQUIRE(mem.read(0xFE00) == 0x10);
	REQUIRE(mem.read(0xFE9F) == 0x20);
	
	REQUIRE(mem.readOAM(0xFE00) == 0x10);
	REQUIRE(mem.readOAM(0xFE9F) == 0x20);
}

TEST_CASE("Memory - OAM blocked returns 0xFF and ignores writes", "[memory]")
{
	Scheduler sched;
	Cartridge cart = makeNoMBCCartridge();
	Memory mem(cart, sched);
	
	mem.setOAMAccessible(true);
	mem.write(0xFE00, 0x77);
	mem.setOAMAccessible(false);
	
	REQUIRE(mem.read(0xFE00) == 0xFF);
	REQUIRE(mem.readOAM(0xFE00) == 0x77); // PPU still sees it.
	
	mem.write(0xFE00, 0x88); // Ignored.
	mem.setOAMAccessible(true);
	REQUIRE(mem.read(0xFE00) == 0x77);
}

TEST_CASE("Memory - Unusable zone returns 0x00 and ignores writes", "[memory]")
{
	Scheduler sched;
	Cartridge cart = makeNoMBCCartridge();
	Memory mem(cart, sched);
	
	REQUIRE(mem.read(0xFEA0) == 0x00);
	REQUIRE(mem.read(0xFEFF) == 0x00);
	
	mem.write(0xFEA0, 0x42);
	mem.write(0xFEFF, 0x99);
	
	REQUIRE(mem.read(0xFEA0) == 0x00);
	REQUIRE(mem.read(0xFEFF) == 0x00);
}

TEST_CASE("Memory - HRAM read/write", "[memory]")
{
	Scheduler sched;
	Cartridge cart = makeNoMBCCartridge();
	Memory mem(cart, sched);
	
	mem.write(0xFF80, 0xAB);
	mem.write(0xFFFE, 0xCD);
	
	REQUIRE(mem.read(0xFF80) == 0xAB);
	REQUIRE(mem.read(0xFFFE) == 0xCD);
}

TEST_CASE("Memory - IE register", "[memory]")
{
	Scheduler sched;
	Cartridge cart = makeNoMBCCartridge();
	Memory mem(cart, sched);
	
	REQUIRE(mem.read(0xFFFF) == 0x00);
	
	mem.write(0xFFFF, 0x1F);
	
	REQUIRE(mem.read(0xFFFF) == 0x1F);
}

TEST_CASE("Memory - DIV write resets it to 0", "[memory]")
{
	Scheduler sched;
	Cartridge cart = makeNoMBCCartridge();
	Memory mem(cart, sched);
	
	REQUIRE(mem.read(0xFF04) == 0xAB); // Power-up.
	
	mem.write(0xFF04, 0x42); // Any value.
	REQUIRE(mem.read(0xFF04) == 0x00);
}

TEST_CASE("Memory - LY is read-only", "[memory]")
{
	Scheduler sched;
	Cartridge cart = makeNoMBCCartridge();
	Memory mem(cart, sched);
	
	REQUIRE(mem.read(0xFF44) == 0x00);
	
	mem.write(0xFF44, 0x90);
	REQUIRE(mem.read(0xFF44) == 0x00); // Unchanged.
}

TEST_CASE("Memory - DMA transfer (instantaneous copy + bus lock)", "[memory]")
{
	Scheduler sched;
	Cartridge cart = makeNoMBCCartridge();
	Memory mem(cart, sched);
	
	// Source pattern in WRAM.
	for(uint16_t i=0;i<0xA0;++i)
		mem.write(0xC000 + i, static_cast<uint8_t>(0xA0 + i));
	
	mem.write(0xFF46, 0xC0); // Starts the DMA.
	
	REQUIRE(mem.isDMAActive());
	REQUIRE(mem.read(0xFE00) == 0xFF);
	
	for(uint16_t i=0;i<0xA0;++i)
		REQUIRE(mem.readOAM(0xFE00 + i) == static_cast<uint8_t>(0xA0 + i));
	
	sched.tick(160 * Scheduler::MCycle);
	REQUIRE_FALSE(mem.isDMAActive());
	
	for(uint16_t i=0;i<0xA0;++i)
		REQUIRE(mem.read(0xFE00 + i) == static_cast<uint8_t>(0xA0 + i));
	
	REQUIRE(mem.read(0xFF46) == 0xC0);
}

TEST_CASE("Memory - DMA works even when OAM is blocked", "[memory]")
{
	Scheduler sched;
	Cartridge cart = makeNoMBCCartridge();
	Memory mem(cart, sched);
	
	for(uint16_t i=0;i<0xA0;++i)
		mem.write(0xC000 + i, static_cast<uint8_t>(i));
	
	mem.setOAMAccessible(false);
	mem.write(0xFF46, 0xC0);
	
	for(uint16_t i=0;i<0xA0;++i)
		REQUIRE(mem.readOAM(0xFE00 + i) == static_cast<uint8_t>(i));
	
	sched.tick(160 * Scheduler::MCycle);
	mem.setOAMAccessible(true);
	
	for(uint16_t i=0;i<0xA0;++i)
		REQUIRE(mem.read(0xFE00 + i) == static_cast<uint8_t>(i));
}

TEST_CASE("Memory - region boundaries", "[memory]")
{
	Scheduler sched;
	Cartridge cart = makeNoMBCCartridge();
	Memory mem(cart, sched);
	
	// Just before / after each major boundary.
	mem.write(0x7FFF, 0x11); // Last ROM byte (write ignored by NoMBC).
	mem.write(0x8000, 0x22); // First VRAM.
	mem.write(0x9FFF, 0x33); // Last VRAM.
	mem.write(0xA000, 0x44); // First Ext RAM (no RAM -> ignored).
	mem.write(0xBFFF, 0x55);
	mem.write(0xC000, 0x66); // First WRAM0.
	mem.write(0xCFFF, 0x77);
	mem.write(0xD000, 0x88); // First WRAM1.
	mem.write(0xDFFF, 0x99);
	
	// Echo writes (do these after checking WRAM if needed, or accept the mirror).
	mem.write(0xE000, 0xAA); // First Echo -> mirrors to 0xC000.
	mem.write(0xFDFF, 0xBB); // Last Echo -> mirrors to 0xDDFF.
	
	mem.write(0xFE00, 0xCC); // First OAM.
	mem.write(0xFE9F, 0xDD); // Last OAM.
	mem.write(0xFF80, 0xEE); // First HRAM.
	mem.write(0xFFFE, 0xFF); // Last HRAM.
	
	// Vérifications
	REQUIRE(mem.read(0x8000) == 0x22);
	REQUIRE(mem.read(0x9FFF) == 0x33);
	
	REQUIRE(mem.read(0xC000) == 0xAA);
	REQUIRE(mem.read(0xCFFF) == 0x77);
	REQUIRE(mem.read(0xD000) == 0x88);
	REQUIRE(mem.read(0xDFFF) == 0x99);
	
	REQUIRE(mem.read(0xE000) == 0xAA);
	REQUIRE(mem.read(0xFDFF) == 0xBB);
	
	REQUIRE(mem.read(0xFE00) == 0xCC);
	REQUIRE(mem.read(0xFE9F) == 0xDD);
	REQUIRE(mem.read(0xFF80) == 0xEE);
	REQUIRE(mem.read(0xFFFE) == 0xFF);
}

TEST_CASE("Memory - DMA is active for 160 M-cycles", "[memory]")
{
	Scheduler sched;
	Cartridge cart = makeNoMBCCartridge();
	Memory mem(cart, sched);
	
	// Fill a source.
	for(uint16_t i=0;i<0xA0;++i)
		mem.write(0xC000 + i, static_cast<uint8_t>(i));
	
	REQUIRE_FALSE(mem.isDMAActive());
	
	mem.write(0xFF46, 0xC0); // démarre le DMA
	
	REQUIRE(mem.isDMAActive());
	
	// During DMA, outside of HRAM -> 0xFF.
	REQUIRE(mem.read(0xC000) == 0xFF);
	REQUIRE(mem.read(0xFE00) == 0xFF);
	
	// HRAM remains accessible.
	mem.write(0xFF80, 0xAB);
	REQUIRE(mem.read(0xFF80) == 0xAB);
	
	// Advance by 639 T-cycles -> still active.
	sched.tick(160 * Scheduler::MCycle - 1);
	REQUIRE(mem.isDMAActive());
	
	// The last cycle -> DMA completed.
	sched.tick(1);
	REQUIRE_FALSE(mem.isDMAActive());
	
	// OAM does indeed contain the data.
	for(uint16_t i=0;i<0xA0;++i)
		REQUIRE(mem.read(0xFE00 + i) == static_cast<uint8_t>(i));
}

TEST_CASE("Memory - DMA can be restarted", "[memory]")
{
	Scheduler sched;
	Cartridge cart = makeNoMBCCartridge();
	Memory mem(cart, sched);
	
	mem.write(0xFF46, 0xC0);
	REQUIRE(mem.isDMAActive());
	
	// Restart before the end.
	mem.write(0xFF46, 0xC0);
	REQUIRE(mem.isDMAActive());
	
	sched.tick(160 * Scheduler::MCycle);
	REQUIRE_FALSE(mem.isDMAActive());
}