#include <vector>

// Helper for creating a ROM with a valid header (reused from test_romheader.cpp).
static inline std::vector<uint8_t> createROMWithHeader(uint8_t mbcCode, uint8_t romSize, uint8_t ramSize, char const* title = "TESTGAME", bool validChecksum = true)
{
	std::vector<uint8_t> rom(0x150, 0x00);
	
	// Entry point (0x0100–0x0103): JP $0150.
	rom[0x0100] = 0xC3;
	rom[0x0101] = 0x50;
	rom[0x0102] = 0x01;
	
	// Logo (0x0104–0x0133): set a non-empty pattern.
	for(int i=0;i<48;++i)
		rom[0x0104 + i] = 0xAA + i;
	
	// Title (0x0134–0x0143).
	size_t len = std::strlen(title);
	
	for(size_t i=0;i<16&&i<len;++i)
		rom[0x0134 + i] = static_cast<uint8_t>(title[i]);
	
	// MBC, sizes.
	rom[0x0147] = mbcCode;
	rom[0x0148] = romSize;
	rom[0x0149] = ramSize;
	
	// Header checksum (0x014D).
	uint8_t sum = 0;
	
	for(uint16_t addr=0x0134;addr<=0x014C;++addr)
		sum += rom[addr];
	
	sum += 0x19;
	rom[0x014D] = validChecksum ? static_cast<uint8_t>(-sum) : 0x00;
	
	// Global checksum (0x014E–0x014F): set to 0.
	rom[0x014E] = 0x00;
	rom[0x014F] = 0x00;
	
	return rom;
}

// For tests with a ROM larger than 0x150, additional data is added.
static inline std::vector<uint8_t> createFullROM(uint8_t mbcCode, uint8_t romSize, uint8_t ramSize, size_t totalSize, char const* title = "TESTGAME", bool validChecksum = true)
{
	std::vector<uint8_t> rom(totalSize, 0x00);
	
	// Copy the header (0x0000–0x014F) from the helper function.
	std::vector<uint8_t> headerRom = createROMWithHeader(mbcCode, romSize, ramSize, title, validChecksum);
	std::copy(headerRom.begin(), headerRom.end(), rom.begin());
	
	// Fill the rest with a recognisable pattern.
	for(size_t i=0x150;i<totalSize;++i)
		rom[i] = static_cast<uint8_t>(i & 0xFF);
	
	return rom;
}