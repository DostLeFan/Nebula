#ifndef DEF_MEMORY
#define DEF_MEMORY

#include <ostream>
#include <istream>
#include <array>
#include <cstdint>
#include "../Cartridge/Cartridge.hpp"
#include "../Scheduler/Scheduler.hpp"

namespace nebula
{
	class Memory
	{
		public:
			explicit Memory(Cartridge& cartridge, Scheduler& scheduler);
			~Memory() = default;
			
			// Generic 16-bit bus access (used by the CPU).
			uint8_t read(uint16_t addr) const;
			void write(uint16_t addr, uint8_t value);
			
			// Direct VRAM access for PPU (bypasses PPU access restrictions).
			uint8_t readVRAM(uint16_t addr) const;
			void writeVRAM(uint16_t addr, uint8_t value);
			
			// Direct OAM access for PPU (bypasses PPU access restrictions).
			uint8_t readOAM(uint16_t addr) const;
			void writeOAM(uint16_t addr, uint8_t value);
			
			// Called by the PPU to lock/unlock VRAM and OAM depending on its current mode (mode 2/3).
			inline void setVRAMAccessible(bool accessible) { m_vramAccessible = accessible; }
			inline void setOAMAccessible(bool accessible) { m_oamAccessible = accessible; }
			
			// For tests / debugging
			bool isDMAActive() const { return m_dmaActive; }
		
		private:
			Cartridge& m_cartridge;
			Scheduler& m_scheduler;
			
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
			
			// DMA state.
			bool m_dmaActive = false;
			Scheduler::EventId m_dmaEventId = 0;
			
			// Internal helpers for I/O.
			uint8_t readIO(uint16_t addr) const;
			void writeIO(uint16_t addr, uint8_t value);
			
			void initIORegisters(); // Power-up values (DMG).
	};
}

#endif // DEF_MEMORY