#include "../../../include/nebula/Cartridge/MBC5.hpp"

#include <algorithm>
#include <stdexcept>

namespace nebula
{
	// Bank 1 by default.
	MBC5::MBC5(std::vector<uint8_t> const& romData, std::vector<uint8_t>& ramData) : m_romData(romData), m_ramData(ramData), m_ramEnabled(false), m_romBank(0x01), m_ramBank(0)
	{
		// Initialize RAM to 0xFF (hardware default).
		std::fill(m_ramData.begin(), m_ramData.end(), 0xFF);
	}
	
	
	uint8_t MBC5::readROM(uint16_t addr) const
	{
		if(addr < 0x4000)
		{
			// Fixed bank 0.
			if(addr >= m_romData.size())
				return 0xFF;
			
			return m_romData[addr];
		}
		
		// Switchable bank: 0x4000–0x7FFF.
		uint16_t bank = m_romBank;
		// Mask to actual ROM size.
		size_t bankCount = getROMBankCount();
		
		if(bankCount > 0)
			bank &= static_cast<uint16_t>(bankCount - 1);
		
		size_t address = (bank * 0x4000 + (addr - 0x4000));
		
		if(address >= m_romData.size())
			return 0xFF;
		
		return m_romData[address];
	}
	
	void MBC5::writeROM(uint16_t addr, uint8_t value)
	{
		if(addr < 0x2000)
			m_ramEnabled = (value == 0x0A); // RAM Enable (0x0000-0x1FFF).
		else if(addr < 0x3000)
			m_romBank = (m_romBank & 0x0100) | value; // Preserve bit 8.
		else if(addr < 0x4000) // ROM Bank Select (bit 8) (0x3000-0x3FFF).
			m_romBank = (m_romBank & 0x00FF) | ((value & 0x01) << 8);
		else if(addr < 0x6000) // RAM Bank Select (bit 8) (0x4000-0x5FFF).
			m_ramBank = value & 0x0F; // 4 bits.
		
		// Other addresses (0x6000-0x7FFF) are ignored.
	}
	
	
	uint8_t MBC5::readRAM(uint16_t addr) const
	{
		if(!m_ramEnabled)
			return 0xFF;
		
		// Calculating the effective RAM bank.
		uint8_t bank = m_ramBank & 0x0F;
		size_t baseAddr = bank * 0x2000;
		size_t offset = addr - 0xA000;
		
		if((baseAddr + offset) >= m_ramData.size())
			return 0xFF; // If the RAM is smaller than the requested bank, return 0xFF.
		
		return m_ramData[(baseAddr + offset)];
	}
	
	void MBC5::writeRAM(uint16_t addr, uint8_t value)
	{
		if(!m_ramEnabled)
			return;
		
		// Calculating the effective RAM bank.
		uint8_t bank = m_ramBank & 0x0F;
		size_t baseAddr = bank * 0x2000;
		size_t offset = addr - 0xA000;
		
		if((baseAddr + offset) < m_ramData.size())
			m_ramData[(baseAddr + offset)] = value;
		
		// Otherwise, ignore the write (RAM not present).
	}
	
	
	void MBC5::saveState(std::ostream& os) const
	{
		// Save the state of the registers.
		uint8_t flags = (m_ramEnabled ? 0x01 : 0x00);
		
		os.write(reinterpret_cast<char const*>(&flags), sizeof(flags));
		os.write(reinterpret_cast<char const*>(&m_romBank), sizeof(m_romBank));
		os.write(reinterpret_cast<char const*>(&m_ramBank), sizeof(m_ramBank));
	}
	
	void MBC5::loadState(std::istream& is)
	{
		uint8_t flags = 0;
		
		is.read(reinterpret_cast<char*>(&flags), sizeof(flags));
		
		m_ramEnabled = (flags & 0x01) != 0;
		
		is.read(reinterpret_cast<char*>(&m_romBank), sizeof(m_romBank));
		is.read(reinterpret_cast<char*>(&m_ramBank), sizeof(m_ramBank));
	}
	
	
	size_t MBC5::getROMBankCount() const
	{
		size_t romSize = m_romData.size();
		
		if(romSize >= 0x8000)
			return romSize / 0x4000; // Number of 16 KB banks.
		
		return 1; // Less than 32 KB (but normally we have at least 32 KB).
	}
}