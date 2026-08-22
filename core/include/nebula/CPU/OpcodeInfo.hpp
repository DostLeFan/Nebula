#ifndef DEF_OPCODEINFO
#define DEF_OPCODEINFO

#include <cstdint>

namespace nebula
{
	/*!
	 * \brief Static metadata for a single CPU opcode (unprefixed or 0xCB-prefixed).
	 * \details
	 * This struct is purely descriptive : it carries no execution logic. The actual
	 * semantics of each opcode are implemented separately in the CPU itself, keyed by
	 * the `opcode` field (see the CPU dispatch table, not yet implemented).
	 * 
	 * The two tables of this type (UnprefixedOpcodeTable and CBPrefixedOpcodeTable) are
	 * generated from the gbdev.io / gbdev/gb-opcodes reference JSON by the
	 * `nebula-opcode-gen` tool (see tools/opcode-gen). Do not edit the generated .cpp
	 * file by hand ; regenerate it instead (see tools/opcode-gen/README.md).
	 */
	struct OpcodeInfo
	{
		uint8_t opcode; // The opcode byte itself (0x00-0xFF). For CB-prefixed entries, this is the byte AFTER the 0xCB prefix.
		char const* mnemonic; // e.g. "LD", "ADD", "JR"... (bare mnemonic, not a full disassembly with operands).
		uint8_t bytes; // Total instruction length in bytes, including the opcode byte itself (but NOT the 0xCB prefix byte for CB-prefixed entries -- see tCyclesTaken below).
		
		/*
			Duration in T-cycles (see Scheduler::MCycle : 1 M-cycle = 4 T-cycles, a fixed
			ratio that never changes, even in CGB double-speed mode -- see project notes).
			All values here are exact multiples of 4, consistent with the fact that every
			SM83 instruction is built out of whole M-cycles.
			
			For CB-prefixed entries, tCyclesTaken/tCyclesNotTaken do NOT include the 4
			T-cycles (1 M-cycle) spent fetching the 0xCB prefix byte itself ; that M-cycle
			is accounted for once by the CPU when it decodes the prefix, not repeated here
			for each of the 256 CB-prefixed opcodes.
		*/
		uint8_t tCyclesTaken; // Duration when a conditional branch IS taken (or the only duration, for non-branching opcodes).
		uint8_t tCyclesNotTaken; // Duration when a conditional branch is NOT taken. Equal to tCyclesTaken for opcodes without a condition.
		
		bool illegal; // True for the 11 undefined opcodes (0xD3, 0xDB, 0xDD, 0xE3, 0xE4, 0xEB, 0xEC, 0xED, 0xF4, 0xFC, 0xFD), which hard-lock the CPU on real hardware.
	};
	
	// 256 entries, indexed directly by the opcode byte (UnprefixedOpcodeTable[opcode]).
	extern OpcodeInfo const UnprefixedOpcodeTable[256];
	
	// 256 entries, indexed by the byte following a 0xCB prefix byte (CBPrefixedOpcodeTable[secondByte]).
	extern OpcodeInfo const CBPrefixedOpcodeTable[256];
}

#endif // DEF_OPCODEINFO
