#include "../../../include/nebula/Cartridge/MBC3.hpp"

#include <algorithm>
#include <stdexcept>
#include <cstdint>

namespace nebula
{
	// Bank 1 by default.
	MBC3::MBC3(std::vector<uint8_t> const& romData, std::vector<uint8_t>& ramData) : m_romData(romData), m_ramData(ramData), m_ramEnabled(false), m_romBank(0x01), m_ramBank(0x00), m_rtcMode(false), m_rtcReg(0x08), m_rtc{}, m_rtcLatched{}, m_latched(false), m_latchPending(false), m_cycleAccumulator(0), m_lastRealTime(std::chrono::system_clock::now())
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
					// m_rtc is already up to date (tick() keeps it current as emulated cycles go by), so there's nothing to catch up here.
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
			uint8_t reg = m_rtcReg;
			
			switch(reg)
			{
				/*
					Real hardware stores the raw byte, with no automatic clamping/wrapping on write ; an "invalid" value (e.g. 60
					seconds) is only corrected once enough ticks accumulate, exactly as incrementOneSecond() naturally does.
				*/
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
		
		// Cycle accumulator: deterministic, portable as-is across machines/runs.
		os.write(reinterpret_cast<char const*>(&m_cycleAccumulator), sizeof(m_cycleAccumulator));
		
		/*
			Wall-clock reference (system_clock epoch seconds), used only by catchUpRealTime(). Stored as a fixed-width int64_t rather than the
			platform-defined std::chrono::seconds::rep, so save files stay portable across machines/compilers.
		*/
		int64_t wallSeconds = std::chrono::duration_cast<std::chrono::seconds>(m_lastRealTime.time_since_epoch()).count();
		os.write(reinterpret_cast<char const*>(&wallSeconds), sizeof(wallSeconds));
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
		
		is.read(reinterpret_cast<char*>(&m_cycleAccumulator), sizeof(m_cycleAccumulator));
		
		int64_t wallSeconds = 0;
		is.read(reinterpret_cast<char*>(&wallSeconds), sizeof(wallSeconds));
		m_lastRealTime = std::chrono::system_clock::time_point(std::chrono::seconds(wallSeconds));
		
		/*
			Deliberately NOT calling catchUpRealTime() here: loadState() must be a pure, deterministic state restore (needed for rewind/rollback, which
			reload states constantly and must never touch the wall clock). Whoever resumes a session (fresh cartridge load, app coming back from the
			background...) is responsible for calling catchUpRealTime() explicitly, exactly once, afterwards.
		*/
	}
	
	
	void MBC3::tick(uint64_t cycles)
	{
		/*
			HALT (bit 6 of daysHigh): on real hardware the RTC counter genuinely stops advancing while halted - time isn't "banked" for later, it's
			simply not counted. So cycles elapsed while halted are dropped here rather than accumulated, which also avoids a large burst catch-up the
			instant HALT is cleared.
		*/
		if(m_rtc.daysHigh & 0x40)
			return;
		
		/*
			Cycle-driven, deterministic advancement: this is what keeps fast-forward (more cycles/host-second -> RTC speeds up in lockstep with the game, which
			is correct) and rewind/save-states (no dependency on wall-clock time) well-behaved. See catchUpRealTime() for the wall-clock-driven counterpart
			used only across sessions.
		*/
		m_cycleAccumulator += cycles;
		
		while(m_cycleAccumulator >= CPU_FREQ_HZ)
		{
			m_cycleAccumulator -= CPU_FREQ_HZ;
			incrementOneSecond();
		}
	}
	
	
	void MBC3::catchUpRealTime()
	{
		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
		std::chrono::seconds elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - m_lastRealTime);
		
		// Clamp negative deltas (system clock moved backwards, e.g. NTP adjustment, or a save file that's inconsistent) to zero rather than underflowing.
		uint64_t secondsElapsed = static_cast<uint64_t>(std::max<std::chrono::seconds::rep>(0, elapsed.count()));
		
		applyElapsedSeconds(secondsElapsed);
		
		/*
			Consume the elapsed real time whether or not it actually got applied (e.g. dropped because HALT was set): time spent halted while the
			emulator wasn't running is lost on real hardware too, not carried forward to the next catch-up.
		*/
		m_lastRealTime = now;
	}
	
	
	void MBC3::incrementOneSecond()
	{
		/*
			Called at most once per emulated second by tick(), so a single-unit carry (rather than a division) is enough - and it naturally corrects
			an "invalid" raw-written value (e.g. seconds=60) after just one tick, matching real hardware.
		*/
		uint16_t seconds = static_cast<uint16_t>(m_rtc.seconds) + 1;
		uint16_t minutes = m_rtc.minutes;
		uint16_t hours = m_rtc.hours;
		uint16_t days = static_cast<uint16_t>(m_rtc.daysLow) | (static_cast<uint16_t>(m_rtc.daysHigh & 0x01) << 8);
		
		if(seconds >= 60) { seconds -= 60; ++minutes; }
		if(minutes >= 60) { minutes -= 60; ++hours; }
		if(hours >= 24) { hours -= 24; ++days; }
		
		if(days > 511)
		{
			days -= 512;
			m_rtc.daysHigh |= 0x80; // Set CARRY flag (bit 7).
		}
		
		m_rtc.seconds = static_cast<uint8_t>(seconds);
		m_rtc.minutes = static_cast<uint8_t>(minutes);
		m_rtc.hours = static_cast<uint8_t>(hours);
		m_rtc.daysLow = static_cast<uint8_t>(days & 0xFF);
		m_rtc.daysHigh = (m_rtc.daysHigh & 0xFE) | static_cast<uint8_t>((days >> 8) & 0x01);
	}
	
	void MBC3::applyElapsedSeconds(uint64_t secondsElapsed)
	{
		// If RTC is halted (bit 6 of daysHigh), do not update.
		if(m_rtc.daysHigh & 0x40)
			return;
		
		if(secondsElapsed == 0)
			return;
		
		// Bulk O(1) carry: needed here (unlike incrementOneSecond()) because a cold catch-up can span a huge gap (days/weeks off), so an O(n) loop of single-second increments isn't acceptable.
		uint64_t totalSeconds = secondsElapsed + m_rtc.seconds;
		uint64_t totalMinutes = m_rtc.minutes + (totalSeconds / 60);
		uint64_t totalHours = m_rtc.hours + (totalMinutes / 60);
		uint64_t totalDays = (m_rtc.daysLow | (static_cast<uint64_t>(m_rtc.daysHigh & 0x01) << 8)) + (totalHours / 24);
		
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