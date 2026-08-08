#include "../../../include/nebula/Cartridge/MBC1.hpp"

#include <algorithm>
#include <stdexcept>

namespace nebula
{
	// Bank 0 transformed to 1 ; default ROM mode is false.
	MBC1::MBC1(std::vector<uint8_t> const& romData, std::vector<uint8_t>& ramData) : m_romData(romData), m_ramData(ramData), m_ramEnabled(false), m_romBank(0x01), m_ramBank(0), m_mode(false) {}
	
	
	uint8_t MBC1::readROM(uint16_t addr) const
	{
		if(addr < 0x4000)
		{
			// Fixed bank 0: 0x0000–0x3FFF.
			if(addr >= m_romData.size())
				return 0xFF;
			
			return m_romData[addr];
		}
		
		// Switchable bank: 0x4000–0x7FFF.
		uint8_t bank = getEffectiveROMBank();
		size_t baseAddr = bank * 0x4000;
		size_t offset = addr - 0x4000;
		size_t finalAddr = (baseAddr + offset);
		
		if(finalAddr >= m_romData.size())
			return 0xFF;
		
		return m_romData[finalAddr];
	}
	
	void MBC1::writeROM(uint16_t addr, uint8_t value)
	{
		if(addr < 0x2000)
			m_ramEnabled = (value == 0x0A); // RAM Enable (0x0000-0x1FFF).
		else if(addr < 0x4000)
		{
			// ROM Bank Select (0x2000-0x3FFF) – 5 bits.
			uint8_t bank = value & 0x1F;
			
			if(bank == 0)
				bank = 0x01; // Bank 0 is prohibited.
			
			m_romBank = bank;
		}
		else if(addr < 0x6000) // ROM/RAM Bank Select (0x4000-0x5FFF) – 2 bits.
			m_ramBank = value & 0x03; // We always store the data, but how it is used depends on the mode.
		else if(addr < 0x8000) // Mode Select (0x6000-0x7FFF).
			m_mode = (value & 0x01) != 0; // 0 = ROM mode, 1 = RAM mode.
		
		// Other entries are ignored.
	}
	
	
	uint8_t MBC1::readRAM(uint16_t addr) const
	{
		if(!m_ramEnabled)
			return 0xFF;
		
		// Calculating the effective RAM bank.
		uint8_t bank;
		
		if(m_mode)
			bank = m_ramBank & 0x03; // RAM mode: bank selected by m_ramBank (0–3).
		else
			bank = 0; // ROM mode: only bank 0 is accessible.
		
		size_t baseAddr = bank * 0x2000;
		size_t offset = addr - 0xA000;
		
		if((baseAddr + offset) >= m_ramData.size())
			return 0xFF; // If the RAM is smaller than the requested bank, return 0xFF.
		
		return m_ramData[(baseAddr + offset)];
	}
	
	void MBC1::writeRAM(uint16_t addr, uint8_t value)
	{
		if(!m_ramEnabled)
			return;
		
		// Calculating the effective RAM bank.
		uint8_t bank;
		
		if(m_mode)
			bank = m_ramBank & 0x03; // RAM mode: bank selected by m_ramBank (0–3).
		else
			bank = 0; // ROM mode: only bank 0 is accessible.
		
		size_t baseAddr = bank * 0x2000;
		size_t offset = addr - 0xA000;
		
		if((baseAddr + offset) < m_ramData.size())
			m_ramData[(baseAddr + offset)] = value;
		
		// Otherwise, ignore the write (RAM not present).
	}
	
	
	void MBC1::saveState(std::ostream& os) const
	{
		// Save the state of the registers.
		uint8_t flags = 0;
		flags |= (m_ramEnabled ? 0x01 : 0x00);
		flags |= (m_mode ? 0x02 : 0x00);
		
		os.write(reinterpret_cast<char const*>(&flags), sizeof(flags));
		os.write(reinterpret_cast<char const*>(&m_romBank), sizeof(m_romBank));
		os.write(reinterpret_cast<char const*>(&m_ramBank), sizeof(m_ramBank));
	}
	
	void MBC1::loadState(std::istream& is)
	{
		uint8_t flags = 0;
		
		is.read(reinterpret_cast<char*>(&flags), sizeof(flags));
		
		m_ramEnabled = (flags & 0x01) != 0;
		m_mode = (flags & 0x02) != 0;
		
		is.read(reinterpret_cast<char*>(&m_romBank), sizeof(m_romBank));
		is.read(reinterpret_cast<char*>(&m_ramBank), sizeof(m_ramBank));
	}
	
	
	uint8_t MBC1::getEffectiveROMBank() const
	{
		uint8_t bank;
		
		if(m_mode)
			bank = m_romBank & 0x1F; // RAM mode: only the 5 bits of m_romBank are used.
		else
			bank = (m_romBank & 0x1F) | ((m_ramBank & 0x03) << 5); // ROM mode: the 2 bits of m_ramBank are used as bits 5–6.
		
		// Mask to adapt to the actual size of the ROM.
		size_t bankCount = getROMBankCount();
		
		if(bankCount > 0)
			bank = static_cast<uint8_t>(bank & (bankCount - 1)); // Because bankCount is a power of 2.
		
		// Bank 0 is never permitted in the switchable zone.
		if(bank == 0)
			bank = 1;
		
		return bank;
	}
	
	size_t MBC1::getROMBankCount() const
	{
		size_t romSize = m_romData.size();
		
		if(romSize >= 0x8000)
			return romSize / 0x4000; // Number of 16 KB banks.
		
		return 1; // Less than 32 KB (but normally we have at least 32 KB).
	}
}