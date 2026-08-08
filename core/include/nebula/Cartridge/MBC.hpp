#ifndef DEF_MBC
#define DEF_MBC

#include <ostream>
#include <istream>
#include <cstdint>

namespace nebula
{
	class MBC
	{
		public:
			virtual ~MBC() = default;
			
			// ROM access (0x0000-0x7FFF).
			virtual uint8_t readROM(uint16_t addr) const = 0;
			virtual void writeROM(uint16_t addr, uint8_t value) = 0;
			
			// External RAM access (0xA000-0xBFFF).
			virtual uint8_t readRAM(uint16_t addr) const = 0;
			virtual void writeRAM(uint16_t addr, uint8_t value) = 0;
			
			// Saving / loading internal state.
			virtual void saveState(std::ostream& os) const = 0;
			virtual void loadState(std::istream& is) = 0;
			
			virtual void tick(uint64_t cycles) { (void)cycles; }
	};
}

#endif // DEF_MBC