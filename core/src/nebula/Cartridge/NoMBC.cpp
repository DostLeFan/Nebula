#include "../../../include/nebula/Cartridge/NoMBC.hpp"

namespace nebula
{
	NoMBC::NoMBC(std::vector<uint8_t> const& romData) : m_romData(romData) {}
	
	
	uint8_t NoMBC::readROM(uint16_t addr) const
	{
		if(addr < m_romData.size())
			return m_romData[addr];
		
		return 0xFF; // Value by default (non-mapped ROM).
	}
	
	void NoMBC::writeROM(uint16_t addr, uint8_t value)
	{
		// Read-only ROM: ignore the writes.
		(void)addr;
		(void)value;
	}
	
	
	uint8_t NoMBC::readRAM(uint16_t addr) const
	{
		(void)addr;
		return 0xFF; // No external RAM.
	}
	
	void NoMBC::writeRAM(uint16_t addr, uint8_t value)
	{
		(void)addr;
		(void)value;
	}
	
	
	void NoMBC::saveState(std::ostream& os) const
	{
		(void)os; // Nothing to save.
	}
	
	void NoMBC::loadState(std::istream& is)
	{
		(void)is; // Nothing to load.
	}
}