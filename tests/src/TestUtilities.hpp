#ifndef TESTUTILITIES_HPP
#define TESTUTILITIES_HPP

#include <vector>
#include <cstdint>
#include <cstddef>

// Helper: create a ROM of variable size with a predictable pattern.
static inline std::vector<uint8_t> createROMWithPattern(size_t size)
{
	std::vector<uint8_t> rom(size, 0x00);
	
	for(size_t i=0;i<size;++i)
		rom[i] = static_cast<uint8_t>(i & 0xFF);
	
	return rom;
}

static inline std::vector<uint8_t> createROMWithBankPattern(size_t size)
{
	std::vector<uint8_t> rom = createROMWithPattern(size);
	constexpr size_t bankSize = 0x4000;
	
	for(size_t offset=0;offset<size;offset+=bankSize)
		rom[offset] = static_cast<uint8_t>((offset / bankSize) & 0xFF); // Marker = bank index (mod 256).
	
	return rom;
}

// Backdates the "last update" timestamp embedded in a MBC3::saveState()
// buffer by `seconds`, so that the next RTC access sees that much elapsed
// real time. This is how we simulate real-time passing (e.g. a save being
// reloaded after the console was off) without actually sleeping in a test.
// Layout written by MBC3::saveState(): flags(1) + romBank(1) + ramBank(1)
// + rtcReg(1) + RTC(5) + RTCLatched(5) + lastUpdate seconds (8, native int64_t).
static inline std::string backdateRTCState(std::string state, int64_t seconds)
{
	constexpr size_t offset = 1 + 1 + 1 + 1 + 5 + 5;
	int64_t ts = 0;
	
	std::memcpy(&ts, state.data() + offset, sizeof(ts));
	
	ts -= seconds;
	
	std::memcpy(&state[offset], &ts, sizeof(ts));
	
	return state;
}

#endif // TESTUTILITIES_HPP