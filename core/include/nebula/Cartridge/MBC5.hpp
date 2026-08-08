#ifndef DEF_MBC5
#define DEF_MBC5

#include <vector>
#include <cstdint>
#include "MBC.hpp"

namespace nebula
{
	/*!
	 * \class MBC5
	 * \brief MBC5 controller.
	 * \details Features:
	 * - ROM up to 8 MB (512 banks of 16 KB)
	 * - External RAM up to 128 KB (16 banks of 8 KB)
	 * - RAM enable/disable via 0x0000-0x1FFF (0x0A enables)
	 * - ROM bank select: 0x2000-0x2FFF (bits 0-7), 0x3000-0x3FFF (bit 8)
	 * - RAM bank select: 0x4000-0x5FFF (4 bits, 0-15)
	 */
	class MBC5 : public MBC
	{
		public:
			MBC5(std::vector<uint8_t> const& romData, std::vector<uint8_t>& ramData);
			
			virtual uint8_t readROM(uint16_t addr) const override;
			virtual void writeROM(uint16_t addr, uint8_t value) override;
			
			virtual uint8_t readRAM(uint16_t addr) const override;
			virtual void writeRAM(uint16_t addr, uint8_t value) override;
			
			virtual void saveState(std::ostream& os) const override;
			virtual void loadState(std::istream& is) override;
		
		private:
			std::vector<uint8_t> const& m_romData;
			std::vector<uint8_t>& m_ramData;
			
			// MBC5 internal registers.
			bool m_ramEnabled;
			uint16_t m_romBank; // 9 bits : ROM bank selection (0-511), bank 0 is replaced by 1 in switchable area.
			uint8_t m_ramBank; // 4 bits : RAM bank selection (0-15).
			
			// Internal methods.
			size_t getROMBankCount() const; // Total number of ROM banks.
	};
}

#endif // DEF_MBC5