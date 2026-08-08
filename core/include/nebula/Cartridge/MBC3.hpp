#ifndef DEF_MBC3
#define DEF_MBC3

#include <vector>
#include <array>
#include <chrono>
#include <cstdint>
#include <ctime>
#include "MBC.hpp"

namespace nebula
{
	/*!
	 * \brief MBC3 controller with Real-Time Clock (RTC).
	 * \details
	 * Features :
	 * - ROM up to 2MB (128 banks of 16 KB) ;
	 * - External RAM up to 32 KB (4 banks of 8 KB) ;
	 * - RTC with seconds, minutes, hours, days (and flags) ;
	 * - Latching mechanism to freeze RTC values during reads.
	 */
	class MBC3 : public MBC
	{
		public:
			MBC3(std::vector<uint8_t> const& romData, std::vector<uint8_t>& ramData);
			~MBC3() override = default;
			
			virtual uint8_t readROM(uint16_t addr) const override;
			virtual void writeROM(uint16_t addr, uint8_t value) override;
			
			virtual uint8_t readRAM(uint16_t addr) const override;
			virtual void writeRAM(uint16_t addr, uint8_t value) override;
			
			virtual void saveState(std::ostream& os) const override;
			virtual void loadState(std::istream& is) override;
			
			virtual void tick(uint64_t cycles) override;
		
		private:
			std::vector<uint8_t> const& m_romData;
			std::vector<uint8_t>& m_ramData;
			
			// MBC3 internal registers.
			bool m_ramEnabled; // RAM/RTC enabled (0x0A written to 0x0000-0x1FFF).
			uint8_t m_romBank; // 7 bits (0-127), bank 0 is converted to 1.
			uint8_t m_ramBank; // 2 bits (0-3) for RAM bank selection.
			bool m_rtcMode; // True if RTC register selected (0x08-0x0C), false if RAM bank selected.
			uint8_t m_rtcReg; // Which RTC register (0x08-0x0C) is selected. Kept separate from m_ramBank because "value & 0x03" collapses 0x08 and 0x0C onto the same 2-bit value, which would otherwise make it impossible to tell the seconds register (0x08) and the days-high register (0x0C) apart.
			
			// RTC registers (values are latched).
			struct RTC
			{
				uint8_t seconds; // 0-59.
				uint8_t minutes; // 0-59.
				uint8_t hours; // 0-23.
				uint8_t daysLow; // Low 8 bits of day counter.
				uint8_t daysHigh; // Bit 0 = day 9th bit, bit 6 = HALT, bit 7 = CARRY.
			};
			
			RTC m_rtc; // Current RTC values.
			RTC m_rtcLatched; // Latched values (after latch sequence).
			bool m_latched; // True if RTC is latched.
			bool m_latchPending; // True if 0x00 has been written, waiting for 0x01 to latch.
			std::chrono::steady_clock::time_point m_lastUpdate; // Last time RTC was updated.
			
			// Real-time clock helpers.
			void updateRTC(); // Called periodically to increment counters.
			void latchRTC(); // Copy current RTC to latched values.
			
			// Internal methods.
			size_t getROMBankCount() const; // Total number of ROM banks.
			bool isRTCSelect(uint8_t value) const;
	};
}

#endif // DEF_MBC3