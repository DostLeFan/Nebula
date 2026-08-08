#ifndef DEF_MEMORY
#define DEF_MEMORY

#include <ostream>
#include <istream>
#include <array>
#include <cstdint>
#include "../Cartridge/Cartridge.hpp"

namespace nebula
{
	class Memory
	{
		public:
			explicit Memory(Cartridge& cartridge);
			~Memory() = default;
			
			// Direct VRAM access for PPU (bypasses PPU access restrictions).
			uint8_t readVRAM(uint16_t addr) const;
			void writeVRAM(uint16_t addr, uint8_t value);
		
		private:
			Cartridge& m_cartridge;
			
			// Internal memory regions.
			std::array<uint8_t, 0x2000> m_wramBank0; // 0xC000-0xCFFF (4 KB).
			std::array<uint8_t, 0x2000> m_wramBank1; // 0xD000-0xDFFF (4 KB).
			std::array<uint8_t, 0x2000> m_vram; // 0x8000-0x9FFF (8 KB).
			std::array<uint8_t, 0xA0> m_oam; // 0xFE00-0xFE9F (160 bytes).
			std::array<uint8_t, 0x80> m_hram; // 0xFF80-0xFFFE (128 bytes).
			std::array<uint8_t, 0x80> m_io; // 0xFF00-0xFF7F (128 bytes).
			uint8_t m_ie = 0; // 0xFFFF.
			
			// PPU access restrictions.
			bool m_vramAccessible = true;
			bool m_oamAccessible = true;
			
			// Internal helpers for I/O.
			uint8_t readIO(uint16_t addr) const;
			void writeIO(uint16_t addr, uint8_t value);
	};
}

#endif // DEF_MEMORY