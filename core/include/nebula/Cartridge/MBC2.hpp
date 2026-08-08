#ifndef DEF_MBC2
#define DEF_MBC2

#include <vector>
#include <array>
#include <cstdint>
#include "MBC.hpp"

namespace nebula
{
	class MBC2 : public MBC
	{
		public:
			MBC2(std::vector<uint8_t> const& romData);
			
			virtual uint8_t readROM(uint16_t addr) const override;
			virtual void writeROM(uint16_t addr, uint8_t value) override;
			
			virtual uint8_t readRAM(uint16_t addr) const override;
			virtual void writeRAM(uint16_t addr, uint8_t value) override;
			
			virtual void saveState(std::ostream& os) const override;
			virtual void loadState(std::istream& is) override;
		
		private:
			std::vector<uint8_t> const& m_romData;
			std::array<uint8_t, 256> m_ram; // 512 × 4 bits (256 bytes).
			
			// MBC2 internal registers.
			bool m_ramEnabled; // RAM activation.
			uint8_t m_romBank; // 0-15.
			
			// Internal methods.
			size_t getROMBankCount() const; // Total number of ROM banks.
	};
}

#endif // DEF_MBC2