#ifndef DEF_NOMBC
#define DEF_NOMBC

#include <vector>
#include "MBC.hpp"

namespace nebula
{
	class NoMBC : public MBC
	{
		public:
			explicit NoMBC(std::vector<uint8_t> const& romData);
			
			virtual uint8_t readROM(uint16_t addr) const override;
			virtual void writeROM(uint16_t addr, uint8_t value) override;
			
			virtual uint8_t readRAM(uint16_t addr) const override;
			virtual void writeRAM(uint16_t addr, uint8_t value) override;
			
			virtual void saveState(std::ostream& os) const override;
			virtual void loadState(std::istream& is) override;
		
		private:
			std::vector<uint8_t> const& m_romData;
	};
}

#endif // DEF_NOMBC