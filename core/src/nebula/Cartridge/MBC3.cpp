#include "../../../include/nebula/Cartridge/MBC3.hpp"

#include <algorithm>
#include <stdexcept>

namespace nebula
{
	// Bank 1 by default.
	MBC3::MBC3(std::vector<uint8_t> const& romData, std::vector<uint8_t>& ramData) : m_romData(romData), m_ramData(ramData), m_ramEnabled(false), m_romBank(0x01), m_ramBank(0x00), m_rtcMode(false), m_rtcReg(0x08), m_rtc{}, m_rtcLatched{}, m_latched(false), m_latchPending(false), m_lastUpdate(std::chrono::steady_clock::now())
	{
		// Initialize RTC with current system time (or default values).
		std::time_t t = std::time(nullptr);
		std::tm* tm = std::localtime(&t);
		
		m_rtc.seconds = static_cast<uint8_t>(tm->tm_sec);
		m_rtc.minutes = static_cast<uint8_t>(tm->tm_min);
		m_rtc.hours = static_cast<uint8_t>(tm->tm_hour);
		m_rtc.daysLow = 0; // Day counter starts at 0.
		m_rtc.daysHigh = 0;
		
		m_rtcLatched = m_rtc;
		
		// Initialize RAM to 0.
		m_ramData.assign(m_ramData.size(), 0x00);
	}
	
	
	uint8_t MBC3::readROM(uint16_t addr) const
	{
		// Fixed bank: 0x0000-0x3FFF.
		if(addr < 0x4000)
		{
			if(addr >= m_romData.size())
				return 0xFF;
			
			return m_romData[addr];
		}
		
		// Switchable bank: 0x4000-0x7FFF.
		size_t bank = m_romBank & 0x7F; // 7 bits (0-127).
		size_t bankCount = getROMBankCount();
		
		// Mask to actual ROM size.
		if(bankCount > 0)
			bank &= static_cast<uint8_t>(bankCount - 1);
		
		if(bank == 0)
			bank = 1; // Bank 0 is prohibited.
		
		size_t address = static_cast<size_t>(bank) * 0x4000 + (addr - 0x4000);
		
		if(address >= m_romData.size())
			return 0xFF;
		
		return m_romData[address];
	}
	
	void MBC3::writeROM(uint16_t addr, uint8_t value)
	{
		if(addr < 0x2000)
			m_ramEnabled = (value == 0x0A); // RAM / RTC Enable (0x0000-0x1FFF). Only 0x0A enables; any other value disables.
		else if(addr < 0x4000)
		{
			// ROM Bank Select (0x2000-0x3FFF).
			// 7 bits (0-127). Bank 0 is not allowed in switchable area.
			uint8_t bank = value & 0x7F;
			
			if(bank == 0)
				bank = 0x01;
			
			m_romBank = bank;
		}
		else if(addr < 0x6000)
		{
			// RAM Bank Select / RTC Register Select (0x4000-0x5FFF).
			// Lower 2 bits select RAM bank (0-3); this is only meaningful
			// outside RTC mode (0x08 and 0x0C would otherwise collide once
			// masked to 2 bits, so the RTC register is tracked separately below).
			m_ramBank = value & 0x03;
			
			// Determine if we are in RTC mode (values 0x08-0x0C inclusive).
			// Values outside this range (0x00-0x07, 0x0D-0xFF) are RAM mode.
			m_rtcMode = (value >= 0x08 && value <= 0x0C);
			
			if(m_rtcMode)
				m_rtcReg = value; // Remember exactly which RTC register (0x08-0x0C) was selected.
		}
		else if(addr < 0x8000)
		{
			// Latch Clock Data (0x6000-0x7FFF).
			// Sequence: write 0x00, then write 0x01 to latch.
			// Any other value resets the pending state.
			if(value == 0x00) // First step: mark latch as pending.
				m_latchPending = true;
			else if(value == 0x01)
			{
				// Second step: if latch is pending, perform latch.
				if(m_latchPending)
				{
					// Update RTC to current time before latching.
					updateRTC();
					latchRTC();
					m_latched = true;
					m_latchPending = false;
				}
			}
			else // Any other value resets the latch pending state.
				m_latchPending = false;
		}
		// Other addresses (0x8000-0xFFFF) are ignored by the cartridge.
	}
	
	
	uint8_t MBC3::readRAM(uint16_t addr) const
	{
		if(!m_ramEnabled)
			return 0xFF;
		
		if(m_rtcMode)
		{
			// RTC mode: m_ramBank (0-3) maps to registers 0x08-0x0C.
			uint8_t reg = m_rtcReg;
			RTC const& rtc = m_latched ? m_rtcLatched : m_rtc;
			
			switch(reg)
			{
				case 0x08: return rtc.seconds;
				case 0x09: return rtc.minutes;
				case 0x0A: return rtc.hours;
				case 0x0B: return rtc.daysLow;
				case 0x0C: return rtc.daysHigh;
				default: return 0xFF;
			}
		}
		else
		{
			// RAM mode.
			uint8_t bank = m_ramBank & 0x03;
			size_t baseAddr = static_cast<size_t>(bank) * 0x2000;
			size_t offset = addr - 0xA000;
			
			if(baseAddr + offset >= m_ramData.size())
				return 0xFF;
			
			return m_ramData[(baseAddr + offset)];
		}
	}
	
	void MBC3::writeRAM(uint16_t addr, uint8_t value)
	{
		if(!m_ramEnabled)
			return;
		
		if(m_rtcMode)
		{
			// Apply elapsed real time before overwriting a register, so that
			// time accumulated in the OTHER fields isn't silently discarded.
			updateRTC();
			
			uint8_t reg = m_rtcReg;
			
			switch(reg)
			{
				// Real hardware stores the raw byte, with no automatic
				// clamping/wrapping on write ; an "invalid" value (e.g. 60
				// seconds) is only corrected by the next real-time tick,
				// exactly as updateRTC() naturally does.
				case 0x08: m_rtc.seconds = value; break;
				case 0x09: m_rtc.minutes = value; break;
				case 0x0A: m_rtc.hours = value; break;
				case 0x0B: m_rtc.daysLow = value; break;
				
				case 0x0C:
					// All bits are writable (bit 6 = HALT, bit 7 = CARRY).
					m_rtc.daysHigh = value;
				break;
				
				default: break;
			}
			
			// Reset last update time: elapsed time was already applied above,
			// so counting restarts from now with the freshly written value.
			m_lastUpdate = std::chrono::steady_clock::now();
			
			// Note: m_rtcLatched is NOT updated here; only m_rtc is modified.
			// If latched, the latched values remain unchanged.
		}
		else
		{
			// RAM mode.
			uint8_t bank = m_ramBank & 0x03;
			size_t baseAddr = static_cast<size_t>(bank) * 0x2000;
			size_t offset = addr - 0xA000;
			
			if(baseAddr + offset < m_ramData.size())
				m_ramData[(baseAddr + offset)] = value;
		}
	}
	
	
	void MBC3::saveState(std::ostream& os) const
	{
		uint8_t flags = 0;
		flags |= (m_ramEnabled ? 0x01 : 0x00);
		flags |= (m_rtcMode ? 0x02 : 0x00);
		flags |= (m_latched ? 0x04 : 0x00);
		
		os.write(reinterpret_cast<char const*>(&flags), sizeof(flags));
		os.write(reinterpret_cast<char const*>(&m_romBank), sizeof(m_romBank));
		os.write(reinterpret_cast<char const*>(&m_ramBank), sizeof(m_ramBank));
		os.write(reinterpret_cast<char const*>(&m_rtcReg), sizeof(m_rtcReg));
		
		os.write(reinterpret_cast<char const*>(&m_rtc), sizeof(m_rtc));
		os.write(reinterpret_cast<char const*>(&m_rtcLatched), sizeof(m_rtcLatched));
		
		std::chrono::steady_clock::duration duration = m_lastUpdate.time_since_epoch();
		std::chrono::seconds::rep seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
		os.write(reinterpret_cast<char const*>(&seconds), sizeof(seconds));
	}
	
	void MBC3::loadState(std::istream& is)
	{
		uint8_t flags;
		is.read(reinterpret_cast<char*>(&flags), sizeof(flags));
		m_ramEnabled = (flags & 0x01) != 0;
		m_rtcMode = (flags & 0x02) != 0;
		m_latched = (flags & 0x04) != 0;
		
		is.read(reinterpret_cast<char*>(&m_romBank), sizeof(m_romBank));
		is.read(reinterpret_cast<char*>(&m_ramBank), sizeof(m_ramBank));
		is.read(reinterpret_cast<char*>(&m_rtcReg), sizeof(m_rtcReg));
		
		is.read(reinterpret_cast<char*>(&m_rtc), sizeof(m_rtc));
		is.read(reinterpret_cast<char*>(&m_rtcLatched), sizeof(m_rtcLatched));
		
		long long seconds;
		is.read(reinterpret_cast<char*>(&seconds), sizeof(seconds));
		std::chrono::seconds duration = std::chrono::seconds(seconds);
		m_lastUpdate = std::chrono::steady_clock::time_point(duration);
	}
	
	
	void MBC3::tick(uint64_t cycles)
	{
		(void)cycles;
		// RTC update will be called from read/write RAM as needed.
		// This method is called by the emulator to keep RTC in sync.
		// We update RTC periodically to avoid lag.
		updateRTC();
	}
	
	
	void MBC3::updateRTC()
	{
		// If RTC is halted (bit 6 of daysHigh), do not update.
		if(m_rtc.daysHigh & 0x40)
			return;
		
		std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
		std::chrono::seconds elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - m_lastUpdate);
		uint32_t secondsElapsed = static_cast<uint32_t>(elapsed.count());
		
		if(secondsElapsed == 0)
			return;
		
		// Accumulate time.
		uint32_t totalSeconds = secondsElapsed + m_rtc.seconds;
		uint32_t totalMinutes = m_rtc.minutes + (totalSeconds / 60);
		uint32_t totalHours = m_rtc.hours + (totalMinutes / 60);
		uint32_t totalDays = (m_rtc.daysLow | (static_cast<uint32_t>(m_rtc.daysHigh & 0x01) << 8)) + (totalHours / 24);
		
		// Apply modulo with explicit casts.
		m_rtc.seconds = static_cast<uint8_t>(totalSeconds % 60);
		m_rtc.minutes = static_cast<uint8_t>(totalMinutes % 60);
		m_rtc.hours = static_cast<uint8_t>(totalHours % 24);
		
		// Day counter: 9 bits (0-511).
		if(totalDays > 511)
		{
			totalDays %= 512;
			m_rtc.daysHigh |= 0x80; // Set CARRY flag (bit 7).
		}
		else
			m_rtc.daysHigh &= ~0x80; // Clear CARRY flag.
		
		m_rtc.daysLow = static_cast<uint8_t>(totalDays & 0xFF);
		m_rtc.daysHigh = (m_rtc.daysHigh & 0xFE) | static_cast<uint8_t>((totalDays >> 8) & 0x01);
		
		m_lastUpdate = now;
	}
	
	void MBC3::latchRTC()
	{
		m_rtcLatched = m_rtc;
	}
	
	
	size_t MBC3::getROMBankCount() const
	{
		size_t romSize = m_romData.size();
		
		if(romSize >= 0x8000)
			return romSize / 0x4000;
		
		return 1; // Less than 32 KB -> single bank.
	}
	
	bool MBC3::isRTCSelect(uint8_t value) const
	{
		return (value >= 0x08 && value <= 0x0C);
	}
}