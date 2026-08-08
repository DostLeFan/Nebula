#ifndef DEF_CARTRIDGE
#define DEF_CARTRIDGE

#include <ostream>
#include <istream>
#include <vector>
#include <memory>
#include <cstdint>
#include "RomHeader.hpp"
#include "MBC.hpp"

namespace nebula
{
	class Cartridge
	{
		public:
			explicit Cartridge(std::vector<uint8_t> const& romData);
			
			inline RomHeader const& getHeader() const { return m_header; } // Access to header (for debug, display, etc.).
			
			// Memory access (delegated to MBC).
			uint8_t read(uint16_t addr) const;
			void write(uint16_t addr, uint8_t value);
			
			inline bool hasBattery() const { return m_header.hasBattery(); } // Indicates whether the cartridge has backup RAM.
			
			// Saving / loading complete state (MBC + external RAM).
			void saveState(std::ostream& os) const;
			void loadState(std::istream& is);
		
		private:
			std::vector<uint8_t> m_romData; // Raw data from ROM.
			std::vector<uint8_t> m_ramData; // External RAM (save).
			RomHeader m_header; // Parsed header.
			std::unique_ptr<MBC> m_mbc; // Bank controller.
			
			void createMBC(); // Instantiates the correct MBC based on the detected type.
	};
}

#endif // DEF_CARTRIDGE