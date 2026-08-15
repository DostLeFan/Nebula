#include "../../../include/nebula/Memory/Memory.hpp"

namespace nebula
{
	Memory::Memory(Cartridge& cartridge, Scheduler& scheduler) : m_cartridge(cartridge), m_scheduler(scheduler)
	{
		m_wramBank0.fill(0);
		m_wramBank1.fill(0);
		m_vram.fill(0);
		m_oam.fill(0);
		m_hram.fill(0);
		m_io.fill(0);
		
		initIORegisters();
	}
	
	
	// Generic 16-bit bus access (used by the CPU).
	uint8_t Memory::read(uint16_t addr) const
	{
		// During a DMA transfer, only HRAM is accessible to the CPU.
		if(m_dmaActive && !(addr >= 0xFF80 && addr <= 0xFFFE))
			return 0xFF;
		
		if(addr < 0x8000) // ROM (0x0000-0x7FFF).
			return m_cartridge.read(addr);
		else if(addr < 0xA000) // VRAM (0x8000-0x9FFF).
			return m_vramAccessible ? m_vram[(addr - 0x8000)] : 0xFF;
		else if(addr < 0xC000) // External RAM (0xA000-0xBFFF).
			return m_cartridge.read(addr);
		else if(addr < 0xD000) // WRAM bank 0 (0xC000-0xCFFF).
			return m_wramBank0[(addr - 0xC000)];
		else if(addr < 0xE000) // WRAM bank 1 (0xD000-0xDFFF).
			return m_wramBank1[(addr - 0xD000)];
		else if(addr < 0xFE00) // Echo RAM (0xE000-0xFDFF) : mirrors 0xC000-0xDDFF.
			return read(addr - 0x2000);
		else if(addr < 0xFEA0) // OAM (0xFE00-0xFE9F).
			return m_oamAccessible ? m_oam[(addr - 0xFE00)] : 0xFF;
		else if(addr < 0xFF00) // Unusable zone (0xFEA0-0xFEFF) - DMG returns 0x00.
			return 0x00;
		else if(addr < 0xFF80) // I/O registers (0xFF00-0xFF7F).
			return readIO(addr);
		else if(addr < 0xFFFF) // HRAM (0xFF80-0xFFFE).
			return m_hram[(addr - 0xFF80)];
		else // IE register (0xFFFF).
			return m_ie;
	}
	
	void Memory::write(uint16_t addr, uint8_t value)
	{
		// During a DMA transfer, only HRAM is accessible.
		if(m_dmaActive && !(addr >= 0xFF80 && addr <= 0xFFFE))
			return;
		
		if(addr < 0x8000) // ROM zone : writing intercepted by the MBC (bank switching, etc.).
			m_cartridge.write(addr, value);
		else if(addr < 0xA000) // VRAM.
		{
			if(m_vramAccessible)
				m_vram[(addr - 0x8000)] = value;
		}
		else if(addr < 0xC000) // External RAM.
			m_cartridge.write(addr, value);
		else if(addr < 0xD000) // WRAM bank 0.
			m_wramBank0[(addr - 0xC000)] = value;
		else if(addr < 0xE000) // WRAM bank 1.
			m_wramBank1[(addr - 0xD000)] = value;
		else if(addr < 0xFE00) // Echo RAM.
			write((addr - 0x2000), value);
		else if(addr < 0xFEA0) // OAM.
		{
			if(m_oamAccessible)
				m_oam[(addr - 0xFE00)] = value;
		}
		else if(addr < 0xFF00) // Unusable zone : writes are ignored.
			return;
		else if(addr < 0xFF80) // I/O registers.
			writeIO(addr, value);
		else if(addr < 0xFFFF) // HRAM.
			m_hram[(addr - 0xFF80)] = value;
		else // IE register.
			m_ie = value;
	}
	
	
	// Direct VRAM access for PPU (bypasses PPU access restrictions).
	uint8_t Memory::readVRAM(uint16_t addr) const
	{
		return m_vram[(addr - 0x8000)]; // Bypasses PPU access restrictions : always allowed (used internally by the PPU itself).
	}
	
	void Memory::writeVRAM(uint16_t addr, uint8_t value)
	{
		m_vram[(addr - 0x8000)] = value;
	}
	
	// Direct OAM access for PPU (bypasses PPU access restrictions).
	uint8_t Memory::readOAM(uint16_t addr) const
	{
		return m_oam[(addr - 0xFE00)]; // Bypasses PPU access restrictions : always allowed (used internally by the PPU itself).
	}
	
	void Memory::writeOAM(uint16_t addr, uint8_t value)
	{
		m_oam[(addr - 0xFE00)] = value;
	}
	
	
	// Internal helpers for I/O.
	uint8_t Memory::readIO(uint16_t addr) const
	{
		return m_io[(addr - 0xFF00)];
	}
	
	void Memory::writeIO(uint16_t addr, uint8_t value)
	{
		switch(addr)
		{
			case 0xFF04: // DIV : writing any value resets the divider to 0.
				m_io[(addr - 0xFF00)] = 0;
			break;
			
			case 0xFF44: // LY : read-only (current scanline, managed by the PPU), writes are ignored.
			break;
			
			case 0xFF46: // DMA : starts an OAM DMA transfer (160 bytes copied from XX00-XX9F to OAM).
			{
				uint16_t source = static_cast<uint16_t>(value) << 8;
				
				for(uint16_t i=0;i<0xA0;++i)
					m_oam[i] = read(static_cast<uint16_t>((source + i))); // Bypasses m_oamAccessible : the DMA controller writes OAM directly.
				
				m_io[(addr - 0xFF00)] = value;
				
				// Cancels any previous DMA.
				if(m_dmaActive)
					m_scheduler.cancel(m_dmaEventId);
				
				m_dmaActive = true;
				
				// DMA ends in 160 M-cycles = 640 T-cycles.
				m_dmaEventId = m_scheduler.schedule(
					160 * Scheduler::MCycle,
					[this]() {
						m_dmaActive = false;
					}
				);
			}
			break;
			
			default:
				m_io[(addr - 0xFF00)] = value;
			break;
		}
	}
	
	void Memory::initIORegisters()
	{
		// Power-up values for DMG (from Pan Docs, at the moment the boot ROM hands over to $0100).
		
		// Joypad / Serial / Timer.
		m_io[0x00] = 0xCF; // P1.
		m_io[0x01] = 0x00; // SB.
		m_io[0x02] = 0x7E; // SC.
		m_io[0x04] = 0xAB; // DIV.
		m_io[0x05] = 0x00; // TIMA.
		m_io[0x06] = 0x00; // TMA.
		m_io[0x07] = 0xF8; // TAC.
		
		// Interrupt Flag.
		m_io[0x0F] = 0xE1; // IF.
		
		// Sound registers.
		m_io[0x10] = 0x80; // NR10.
		m_io[0x11] = 0xBF; // NR11.
		m_io[0x12] = 0xF3; // NR12.
		m_io[0x13] = 0xFF; // NR13.
		m_io[0x14] = 0xBF; // NR14.
		m_io[0x16] = 0x3F; // NR21.
		m_io[0x17] = 0x00; // NR22.
		m_io[0x18] = 0xFF; // NR23.
		m_io[0x19] = 0xBF; // NR24.
		m_io[0x1A] = 0x7F; // NR30.
		m_io[0x1B] = 0xFF; // NR31.
		m_io[0x1C] = 0x9F; // NR32.
		m_io[0x1D] = 0xFF; // NR33.
		m_io[0x1E] = 0xBF; // NR34.
		m_io[0x20] = 0xFF; // NR41.
		m_io[0x21] = 0x00; // NR42.
		m_io[0x22] = 0x00; // NR43.
		m_io[0x23] = 0xBF; // NR44.
		m_io[0x24] = 0x77; // NR50.
		m_io[0x25] = 0xF3; // NR51.
		m_io[0x26] = 0xF1; // NR52.
		
		// LCD.
		m_io[0x40] = 0x91; // LCDC.
		m_io[0x41] = 0x85; // STAT.
		m_io[0x42] = 0x00; // SCY.
		m_io[0x43] = 0x00; // SCX.
		m_io[0x44] = 0x00; // LY.
		m_io[0x45] = 0x00; // LYC.
		m_io[0x46] = 0xFF; // DMA.
		m_io[0x47] = 0xFC; // BGP.
		m_io[0x48] = 0xFF; // OBP0.
		m_io[0x49] = 0xFF; // OBP1.
		m_io[0x4A] = 0x00; // WY.
		m_io[0x4B] = 0x00; // WX.
	}
}