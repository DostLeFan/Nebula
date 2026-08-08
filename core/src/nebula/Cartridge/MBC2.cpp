#include "../../../include/nebula/Cartridge/MBC2.hpp"

#include <algorithm>
#include <stdexcept>

namespace nebula
{
	// Bank 1 by default.
	MBC2::MBC2(std::vector<uint8_t> const& romData) : m_romData(romData), m_ram{}, m_ramEnabled(false), m_romBank(0x01)
	{
		m_ram.fill(0x00);
	}
	
	
	uint8_t MBC2::readROM(uint16_t addr) const
	{
		if(addr < 0x4000)
		{
			if(addr >= m_romData.size())
				return 0xFF;
			
			return m_romData[addr];
		}
		
		size_t bank = m_romBank & 0x0F;
		
		// Mask for the size of the ROM.
		size_t bankCount = getROMBankCount();
		
		if(bankCount > 0)
			bank &= (bankCount - 1);
		
		if(bank == 0)
			bank = 1; // Bank 0 is prohibited.
		
		size_t address = (bank * 0x4000 + (addr - 0x4000));
		
		if(address >= m_romData.size())
			return 0xFF;
		
		return m_romData[address];
	}
	
	void MBC2::writeROM(uint16_t addr, uint8_t value)
	{
		// Check bit 8 of the address (0x0100).
		if((addr & 0x0100) == 0) // RAM Enable (0x0000–0x1FFF): only bit 4 of the value matters.
			m_ramEnabled = ((value & 0x0F) == 0x0A); // 0x0A enabled.
		else
		{
			// ROM Bank Select (0x2100-0x3FFF) : bits 0-3.
			uint8_t bank = value & 0x0F;
			
			if(bank == 0) bank = 0x01;
				m_romBank = bank;
		}
		
		// All other entries are ignored.
	}
	
	
	uint8_t MBC2::readRAM(uint16_t addr) const
	{
		if(!m_ramEnabled)
			return 0xFF;
		
		if(addr < 0xA000 || addr > 0xA1FF)
			return 0xFF;
		
		// Only the 4 least significant bits are significant for reading.
		uint8_t value = m_ram[addr - 0xA000];
		
		return value & 0x0F; // Only 4 bits.
	}
	
	void MBC2::writeRAM(uint16_t addr, uint8_t value)
	{
		if(!m_ramEnabled)
			return;
		
		if(addr < 0xA000 || addr > 0xA1FF)
			return;
		
		// Only the 4 least significant bits are stored.
		m_ram[addr - 0xA000] = value & 0x0F;
	}
	
	
	void MBC2::saveState(std::ostream& os) const
	{
		uint8_t flags = m_ramEnabled ? 0x01 : 0x00;
		
		os.write(reinterpret_cast<char const*>(&flags), sizeof(flags));
		os.write(reinterpret_cast<char const*>(&m_romBank), sizeof(m_romBank));
		os.write(reinterpret_cast<char const*>(m_ram.data()), m_ram.size());
	}
	
	void MBC2::loadState(std::istream& is)
	{
		uint8_t flags;
		
		is.read(reinterpret_cast<char*>(&flags), sizeof(flags));
		
		m_ramEnabled = (flags & 0x01) != 0;
		
		is.read(reinterpret_cast<char*>(&m_romBank), sizeof(m_romBank));
		is.read(reinterpret_cast<char*>(m_ram.data()), m_ram.size());
	}
	
	
	size_t MBC2::getROMBankCount() const
	{
		size_t romSize = m_romData.size();
		
		if(romSize >= 0x8000)
			return romSize / 0x4000;
		
		return 1;
	}
}