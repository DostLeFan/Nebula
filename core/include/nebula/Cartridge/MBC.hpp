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
			
			/*
				Applies real (wall-clock) time elapsed since the last call, in a single step. Meant to be called once per "cold" transition - typically right
				after loadState() when resuming a session - NOT on every emulated step. No-op by default: only MBCs with an actual real-time clock (currently
				MBC3) need to override it.
			*/
			virtual void catchUpRealTime() { }
	};
}

#endif // DEF_MBC