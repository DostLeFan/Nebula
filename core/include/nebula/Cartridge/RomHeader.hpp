#ifndef DEF_ROMHEADER
#define DEF_ROMHEADER

#include <string>
#include <vector>
#include <array>
#include <cstdint>

namespace nebula
{
	enum class MBCType : uint8_t
	{
		NONE = 0x00,
		MBC1 = 0x01,
		MBC2 = 0x05,
		MBC3 = 0x0F,
		MBC5 = 0x19,
		
		// Other can be added later (MB7, etc.).
	};
	
	struct RomHeader
	{
		// Constructor : parse ROM data.
		explicit RomHeader(std::vector<uint8_t> const& romData);
		
		// Public fields, extracted from header.
		uint16_t entryPoint = 0; // Entry point (0x0100-0x0103).
		std::array<uint8_t, 48> logo{}; // Nintendo logo (optional) (0x0104-0x0133).
		std::array<char, 16> title{}; // Title (0x0134-0x0143).
		MBCType mbcType = MBCType::NONE; // MBC type (0x0147).
		uint8_t mbcRawCode = 0; // MBC Type (original code) (0x0147).
		uint8_t romSizeCode = 0; // ROM size code (0x0148).
		uint8_t ramSizeCode = 0; // RAM size code (0x0149).
		uint8_t checksumHeader = 0; // Header checksum (0x014D).
		uint8_t checksumGlobal = 0; // Global checksum (0x014E-0x014F).
		
		size_t getTotalROMSize() const;
		size_t getTotalRAMSize() const;
		bool verifyLogo() const; // Verify that logo is not empty (at least one not null byte). In reality, it's better to check if it's an official Nintendo logo.
		bool hasBattery() const; // True if RAM is saved by battery.
		bool isValid() const; // Basic verification of header.
		
		private:
			std::vector<uint8_t> const* m_romData; // Reference to raw data.
			void parse(std::vector<uint8_t> const& data);
	};
}

#endif // DEF_ROMHEADER