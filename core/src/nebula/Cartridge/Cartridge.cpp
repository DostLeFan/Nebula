#include "../../../include/nebula/Cartridge/Cartridge.hpp"

#include <algorithm>
#include <stdexcept>
#include "../../../include/nebula/Cartridge/NoMBC.hpp"
#include "../../../include/nebula/Cartridge/MBC1.hpp"
#include "../../../include/nebula/Cartridge/MBC2.hpp"
#include "../../../include/nebula/Cartridge/MBC3.hpp"

namespace nebula
{
	Cartridge::Cartridge(std::vector<uint8_t> const& romData) : m_romData(romData), m_header(romData)
	{
		size_t ramSize = m_header.getTotalRAMSize();
		
		if(ramSize > 0)
			m_ramData.resize(ramSize, 0xFF); // Often initializated at 0xFF.
		
		createMBC();
	}
	
	
	uint8_t Cartridge::read(uint16_t addr) const
	{
		if(addr < 0x8000) // ROM zone (0x0000-0x7FFF).
			return m_mbc->readROM(addr);
		else if(addr >= 0xA000 && addr < 0xC000) // RAM zone (0xA000-0xBFFF).
			return m_mbc->readRAM(addr);
		
		return 0xFF; // Other zones aren't managed by the cartridge.
	}
	
	void Cartridge::write(uint16_t addr, uint8_t value)
	{
		if(addr < 0x8000) // ROM zone (0x0000-0x7FFF) : writing intercepted by the MBC.
			m_mbc->writeROM(addr, value);
		else if(addr >= 0xA000 && addr < 0xC000) // RAM zone (0xA000-0xBFFF).
			m_mbc->writeRAM(addr, value);
		
		// The other areas are ignored by the cartridge.
	}
	
	
	void Cartridge::saveState(std::ostream& os) const
	{
		// Save MBC state.
		m_mbc->saveState(os);
		
		
		// Save external RAM.
		uint32_t ramSize = static_cast<uint32_t>(m_ramData.size());
		
		os.write(reinterpret_cast<char const*>(&ramSize), sizeof(ramSize));
		
		if(ramSize > 0)
			os.write(reinterpret_cast<char const*>(m_ramData.data()), ramSize);
	}
	
	void Cartridge::loadState(std::istream& is)
	{
		// Load MBC state.
		m_mbc->loadState(is);
		
		// Load external RAM.
		uint32_t ramSize = static_cast<uint32_t>(m_ramData.size());
		
		is.read(reinterpret_cast<char*>(&ramSize), sizeof(ramSize));
		
		if(ramSize > 0)
		{
			if(ramSize == m_ramData.size()) // Check that the size is correct.
				is.read(reinterpret_cast<char*>(m_ramData.data()), ramSize);
			else // If the size doesn't match, it is ignored.
				is.seekg(ramSize, std::ios::cur); // Ignore data.
		}
	}
	
	
	void Cartridge::createMBC()
	{
		switch(m_header.mbcType)
		{
			case MBCType::NONE:
				m_mbc = std::make_unique<NoMBC>(m_romData);
			break;
			
			case MBCType::MBC1:
				m_mbc = std::make_unique<MBC1>(m_romData, m_ramData);
			break;
			
			case MBCType::MBC2:
				m_mbc = std::make_unique<MBC2>(m_romData);
			break;
			
			case MBCType::MBC3:
				m_mbc = std::make_unique<MBC3>(m_romData, m_ramData);
			break;
			
			/*case MBCType::MBC5:
				m_mbc = std::make_unique<MBC5>(m_romData, m_ramData);
			break;*/
			
			default: // Fallback: NoMBC (some games may not run).
				m_mbc = std::make_unique<NoMBC>(m_romData);
			break;
		}
	}
}