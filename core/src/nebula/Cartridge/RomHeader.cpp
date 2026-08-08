#include "../../../include/nebula/Cartridge/RomHeader.hpp"

#include <stdexcept>
#include <cstring>

namespace nebula
{
	// Constructor : parse ROM data.
	RomHeader::RomHeader(std::vector<uint8_t> const& romData) : m_romData(&romData)
	{
		if(romData.size() < 0x150)
			throw std::runtime_error("ROM too small: header extends beyond end of file");
		
		parse(romData);
	}
	
	
	bool RomHeader::verifyLogo() const
	{
		for(uint8_t b : logo)
		{
			if(b != 0)
				return true;
		}
		
		return false;
	}
	
	size_t RomHeader::getTotalROMSize() const
	{
		// Standard codes: 0x00–0x08 -> 32 KB * 2^n.
		if(romSizeCode <= 0x08)
			return static_cast<size_t>(32 * 1024) << romSizeCode;
		
		// Special (non-standard) codes.
		switch (romSizeCode)
		{
			case 0x52: return 1152 * 1024; // 1.125 Mo
			case 0x53: return 1280 * 1024; // 1.25 Mo
			case 0x54: return 1536 * 1024; // 1.5 Mo
			default: return 0;
		}
	}
	
	size_t RomHeader::getTotalRAMSize() const
	{
		switch(ramSizeCode)
		{
			case 0x00: return 0; // No RAM.
			case 0x01: return 2 * 1024; // 2 Ko.
			case 0x02: return 8 * 1024; // 8 Ko.
			case 0x03: return 32 * 1024; // 32 Ko.
			case 0x04: return 128 * 1024; // 127 Ko.
			case 0x05: return 64 * 1024; // 64 Ko.
			default: return 0; // Unknown.
		}
	}
	
	bool RomHeader::hasBattery() const
	{
		switch(mbcRawCode)
		{
			case 0x03: // MBC1+RAM+Battery
			case 0x06: // MBC2+Battery
			case 0x0F: // MBC3+RAM+Battery
			case 0x10: // MBC3+RAM+Battery+RTC
			case 0x11: // MBC3+RAM+Battery+RTC
			case 0x12: // MBC3+RAM+Battery+RTC
			case 0x13: // MBC3+RAM+Battery+RTC
			case 0x1B: // MBC5+RAM+Battery
			case 0x1C: // MBC5+RTC
			case 0x1D: // MBC5+RAM+Battery+RTC
			case 0x1E: // MBC5+RAM+Battery+RTC
				return true;
			default:
				return false;
		}
	}
	
	bool RomHeader::isValid() const
	{
		if(!m_romData)
			return false;
		
		// Checking the entry point (must be 0xC3 0x50 0x01).
		if((*m_romData)[0x0100] != 0xC3 || (*m_romData)[0x0101] != 0x50 || (*m_romData)[0x0102] != 0x01)
			return false;
		
		// Verification of the header checksum (0x0134–0x014C).
		uint8_t sum = 0;
		
		for(uint16_t addr=0x0134;addr<=0x014C;++addr)
			sum += (*m_romData)[addr];
		
		sum += 0x19;
		sum += checksumHeader;
		
		if(sum != 0)
			return false;
		
		// Basic check for the logo (not empty).
		if(!verifyLogo())
			return false;
		
		return true;
	}
	
	
	void RomHeader::parse(std::vector<uint8_t> const& data)
	{
		// Entry point (0x0100-0x0103).
		entryPoint = (static_cast<uint16_t>(data[0x0102]) << 8) | data[0x0101];
		
		// Nintendo logo (optional) (0x0104-0x0133).
		for(int i=0;i<48;++i)
			logo[i] = data[(0x0104 + i)];
		
		// Title (0x0134-0x0143).
		for(int i=0;i<16;++i)
			title[i] = static_cast<char>(data[(0x0134 + i)]);
		
		// MBC Type (original code) (0x0147).
		mbcRawCode = data[0x0147];
		
		switch(mbcRawCode)
		{
			case 0x00: mbcType = MBCType::NONE; break;
			
			case 0x01: // MBC1
			case 0x02: // MBC1+RAM
			case 0x03: // MBC1+RAM+Battery
				mbcType = MBCType::MBC1;
			break;
			
			case 0x05: // MBC2
			case 0x06: // MBC2+Battery
				mbcType = MBCType::MBC2;
			break;
			
			case 0x0F: // MBC3
			case 0x10: // MBC3+RAM
			case 0x11: // MBC3+RAM+Battery
			case 0x12: // MBC3+RAM+Battery+RTC
			case 0x13: // MBC3+RAM+Battery+RTC
				mbcType = MBCType::MBC3;
			break;
			
			case 0x19: // MBC5
			case 0x1A: // MBC5+RAM
			case 0x1B: // MBC5+RAM+Battery
			case 0x1C: // MBC5+RTC
			case 0x1D: // MBC5+RAM+Battery+RTC
			case 0x1E: // MBC5+RAM+Battery+RTC
				mbcType = MBCType::MBC5;
			break;
			
			default:
				mbcType = MBCType::NONE;
			break;
		}
		
		// ROM size code (0x0148).
		romSizeCode = data[0x0148];
		
		// RAM size code (0x0149).
		ramSizeCode = data[0x0149];
		
		// Header checksum (0x014D).
		checksumHeader = data[0x014D];
		
		// Global checksum (0x014E-0x014F).
		checksumGlobal = (static_cast<uint16_t>(data[0x014E]) << 8) | data[0x014F];
	}
}