#ifndef DEF_MBC1
#define DEF_MBC1

#include <vector>
#include <cstdint>
#include "MBC.hpp"

namespace nebula
{
	class MBC1 : public MBC
	{
		public:
			MBC1(std::vector<uint8_t> const& romData, std::vector<uint8_t>& ramData);
			
			virtual uint8_t readROM(uint16_t addr) const override;
			virtual void writeROM(uint16_t addr, uint8_t value) override;
			
			virtual uint8_t readRAM(uint16_t addr) const override;
			virtual void writeRAM(uint16_t addr, uint8_t value) override;
			
			virtual void saveState(std::ostream& os) const override;
			virtual void loadState(std::istream& is) override;
		
		private:
			std::vector<uint8_t> const& m_romData;
			std::vector<uint8_t>& m_ramData;
			
			// MBC1 internal registers.
			bool m_ramEnabled; // RAM activation (0x0A in 0x0000-0x1FFF).
			uint8_t m_romBank; // 5 bits : ROM bank selection (0x01-0x1F).
			uint8_t m_ramBank; // 2 bits : RAM bank selection (0-3).
			bool m_mode; // 0 = ROM mode, 1 = RAM mode.
			
			// Internal methods.
			uint8_t getEffectiveROMBank() const; // Compute ROM bank according to mode.
			size_t getROMBankCount() const; // Total number of ROM banks.
	};
}

#endif // DEF_MBC1