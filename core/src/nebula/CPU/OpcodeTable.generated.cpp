// GENERATED FILE - DO NOT EDIT BY HAND.
// Regenerate with the `regen_opcodes` CMake target (see tools/opcode-gen).
// Source: https://github.com/gbdev/gb-opcodes (Opcodes.json)

#include "../../../include/nebula/CPU/OpcodeInfo.hpp"

namespace nebula
{
	OpcodeInfo const UnprefixedOpcodeTable[256] =
	{
		{ 0x00, "NOP", 1, 4, 4, false }, // NOP
		{ 0x01, "LD", 3, 12, 12, false }, // LD BC, n16
		{ 0x02, "LD", 1, 8, 8, false }, // LD (BC), A
		{ 0x03, "INC", 1, 8, 8, false }, // INC BC
		{ 0x04, "INC", 1, 4, 4, false }, // INC B
		{ 0x05, "DEC", 1, 4, 4, false }, // DEC B
		{ 0x06, "LD", 2, 8, 8, false }, // LD B, n8
		{ 0x07, "RLCA", 1, 4, 4, false }, // RLCA
		{ 0x08, "LD", 3, 20, 20, false }, // LD (a16), SP
		{ 0x09, "ADD", 1, 8, 8, false }, // ADD HL, BC
		{ 0x0A, "LD", 1, 8, 8, false }, // LD A, (BC)
		{ 0x0B, "DEC", 1, 8, 8, false }, // DEC BC
		{ 0x0C, "INC", 1, 4, 4, false }, // INC C
		{ 0x0D, "DEC", 1, 4, 4, false }, // DEC C
		{ 0x0E, "LD", 2, 8, 8, false }, // LD C, n8
		{ 0x0F, "RRCA", 1, 4, 4, false }, // RRCA
		{ 0x10, "STOP", 2, 4, 4, false }, // STOP n8
		{ 0x11, "LD", 3, 12, 12, false }, // LD DE, n16
		{ 0x12, "LD", 1, 8, 8, false }, // LD (DE), A
		{ 0x13, "INC", 1, 8, 8, false }, // INC DE
		{ 0x14, "INC", 1, 4, 4, false }, // INC D
		{ 0x15, "DEC", 1, 4, 4, false }, // DEC D
		{ 0x16, "LD", 2, 8, 8, false }, // LD D, n8
		{ 0x17, "RLA", 1, 4, 4, false }, // RLA
		{ 0x18, "JR", 2, 12, 12, false }, // JR e8
		{ 0x19, "ADD", 1, 8, 8, false }, // ADD HL, DE
		{ 0x1A, "LD", 1, 8, 8, false }, // LD A, (DE)
		{ 0x1B, "DEC", 1, 8, 8, false }, // DEC DE
		{ 0x1C, "INC", 1, 4, 4, false }, // INC E
		{ 0x1D, "DEC", 1, 4, 4, false }, // DEC E
		{ 0x1E, "LD", 2, 8, 8, false }, // LD E, n8
		{ 0x1F, "RRA", 1, 4, 4, false }, // RRA
		{ 0x20, "JR", 2, 12, 8, false }, // JR NZ, e8
		{ 0x21, "LD", 3, 12, 12, false }, // LD HL, n16
		{ 0x22, "LD", 1, 8, 8, false }, // LD (HL)+, A
		{ 0x23, "INC", 1, 8, 8, false }, // INC HL
		{ 0x24, "INC", 1, 4, 4, false }, // INC H
		{ 0x25, "DEC", 1, 4, 4, false }, // DEC H
		{ 0x26, "LD", 2, 8, 8, false }, // LD H, n8
		{ 0x27, "DAA", 1, 4, 4, false }, // DAA
		{ 0x28, "JR", 2, 12, 8, false }, // JR Z, e8
		{ 0x29, "ADD", 1, 8, 8, false }, // ADD HL, HL
		{ 0x2A, "LD", 1, 8, 8, false }, // LD A, (HL)+
		{ 0x2B, "DEC", 1, 8, 8, false }, // DEC HL
		{ 0x2C, "INC", 1, 4, 4, false }, // INC L
		{ 0x2D, "DEC", 1, 4, 4, false }, // DEC L
		{ 0x2E, "LD", 2, 8, 8, false }, // LD L, n8
		{ 0x2F, "CPL", 1, 4, 4, false }, // CPL
		{ 0x30, "JR", 2, 12, 8, false }, // JR NC, e8
		{ 0x31, "LD", 3, 12, 12, false }, // LD SP, n16
		{ 0x32, "LD", 1, 8, 8, false }, // LD (HL)-, A
		{ 0x33, "INC", 1, 8, 8, false }, // INC SP
		{ 0x34, "INC", 1, 12, 12, false }, // INC (HL)
		{ 0x35, "DEC", 1, 12, 12, false }, // DEC (HL)
		{ 0x36, "LD", 2, 12, 12, false }, // LD (HL), n8
		{ 0x37, "SCF", 1, 4, 4, false }, // SCF
		{ 0x38, "JR", 2, 12, 8, false }, // JR C, e8
		{ 0x39, "ADD", 1, 8, 8, false }, // ADD HL, SP
		{ 0x3A, "LD", 1, 8, 8, false }, // LD A, (HL)-
		{ 0x3B, "DEC", 1, 8, 8, false }, // DEC SP
		{ 0x3C, "INC", 1, 4, 4, false }, // INC A
		{ 0x3D, "DEC", 1, 4, 4, false }, // DEC A
		{ 0x3E, "LD", 2, 8, 8, false }, // LD A, n8
		{ 0x3F, "CCF", 1, 4, 4, false }, // CCF
		{ 0x40, "LD", 1, 4, 4, false }, // LD B, B
		{ 0x41, "LD", 1, 4, 4, false }, // LD B, C
		{ 0x42, "LD", 1, 4, 4, false }, // LD B, D
		{ 0x43, "LD", 1, 4, 4, false }, // LD B, E
		{ 0x44, "LD", 1, 4, 4, false }, // LD B, H
		{ 0x45, "LD", 1, 4, 4, false }, // LD B, L
		{ 0x46, "LD", 1, 8, 8, false }, // LD B, (HL)
		{ 0x47, "LD", 1, 4, 4, false }, // LD B, A
		{ 0x48, "LD", 1, 4, 4, false }, // LD C, B
		{ 0x49, "LD", 1, 4, 4, false }, // LD C, C
		{ 0x4A, "LD", 1, 4, 4, false }, // LD C, D
		{ 0x4B, "LD", 1, 4, 4, false }, // LD C, E
		{ 0x4C, "LD", 1, 4, 4, false }, // LD C, H
		{ 0x4D, "LD", 1, 4, 4, false }, // LD C, L
		{ 0x4E, "LD", 1, 8, 8, false }, // LD C, (HL)
		{ 0x4F, "LD", 1, 4, 4, false }, // LD C, A
		{ 0x50, "LD", 1, 4, 4, false }, // LD D, B
		{ 0x51, "LD", 1, 4, 4, false }, // LD D, C
		{ 0x52, "LD", 1, 4, 4, false }, // LD D, D
		{ 0x53, "LD", 1, 4, 4, false }, // LD D, E
		{ 0x54, "LD", 1, 4, 4, false }, // LD D, H
		{ 0x55, "LD", 1, 4, 4, false }, // LD D, L
		{ 0x56, "LD", 1, 8, 8, false }, // LD D, (HL)
		{ 0x57, "LD", 1, 4, 4, false }, // LD D, A
		{ 0x58, "LD", 1, 4, 4, false }, // LD E, B
		{ 0x59, "LD", 1, 4, 4, false }, // LD E, C
		{ 0x5A, "LD", 1, 4, 4, false }, // LD E, D
		{ 0x5B, "LD", 1, 4, 4, false }, // LD E, E
		{ 0x5C, "LD", 1, 4, 4, false }, // LD E, H
		{ 0x5D, "LD", 1, 4, 4, false }, // LD E, L
		{ 0x5E, "LD", 1, 8, 8, false }, // LD E, (HL)
		{ 0x5F, "LD", 1, 4, 4, false }, // LD E, A
		{ 0x60, "LD", 1, 4, 4, false }, // LD H, B
		{ 0x61, "LD", 1, 4, 4, false }, // LD H, C
		{ 0x62, "LD", 1, 4, 4, false }, // LD H, D
		{ 0x63, "LD", 1, 4, 4, false }, // LD H, E
		{ 0x64, "LD", 1, 4, 4, false }, // LD H, H
		{ 0x65, "LD", 1, 4, 4, false }, // LD H, L
		{ 0x66, "LD", 1, 8, 8, false }, // LD H, (HL)
		{ 0x67, "LD", 1, 4, 4, false }, // LD H, A
		{ 0x68, "LD", 1, 4, 4, false }, // LD L, B
		{ 0x69, "LD", 1, 4, 4, false }, // LD L, C
		{ 0x6A, "LD", 1, 4, 4, false }, // LD L, D
		{ 0x6B, "LD", 1, 4, 4, false }, // LD L, E
		{ 0x6C, "LD", 1, 4, 4, false }, // LD L, H
		{ 0x6D, "LD", 1, 4, 4, false }, // LD L, L
		{ 0x6E, "LD", 1, 8, 8, false }, // LD L, (HL)
		{ 0x6F, "LD", 1, 4, 4, false }, // LD L, A
		{ 0x70, "LD", 1, 8, 8, false }, // LD (HL), B
		{ 0x71, "LD", 1, 8, 8, false }, // LD (HL), C
		{ 0x72, "LD", 1, 8, 8, false }, // LD (HL), D
		{ 0x73, "LD", 1, 8, 8, false }, // LD (HL), E
		{ 0x74, "LD", 1, 8, 8, false }, // LD (HL), H
		{ 0x75, "LD", 1, 8, 8, false }, // LD (HL), L
		{ 0x76, "HALT", 1, 4, 4, false }, // HALT
		{ 0x77, "LD", 1, 8, 8, false }, // LD (HL), A
		{ 0x78, "LD", 1, 4, 4, false }, // LD A, B
		{ 0x79, "LD", 1, 4, 4, false }, // LD A, C
		{ 0x7A, "LD", 1, 4, 4, false }, // LD A, D
		{ 0x7B, "LD", 1, 4, 4, false }, // LD A, E
		{ 0x7C, "LD", 1, 4, 4, false }, // LD A, H
		{ 0x7D, "LD", 1, 4, 4, false }, // LD A, L
		{ 0x7E, "LD", 1, 8, 8, false }, // LD A, (HL)
		{ 0x7F, "LD", 1, 4, 4, false }, // LD A, A
		{ 0x80, "ADD", 1, 4, 4, false }, // ADD A, B
		{ 0x81, "ADD", 1, 4, 4, false }, // ADD A, C
		{ 0x82, "ADD", 1, 4, 4, false }, // ADD A, D
		{ 0x83, "ADD", 1, 4, 4, false }, // ADD A, E
		{ 0x84, "ADD", 1, 4, 4, false }, // ADD A, H
		{ 0x85, "ADD", 1, 4, 4, false }, // ADD A, L
		{ 0x86, "ADD", 1, 8, 8, false }, // ADD A, (HL)
		{ 0x87, "ADD", 1, 4, 4, false }, // ADD A, A
		{ 0x88, "ADC", 1, 4, 4, false }, // ADC A, B
		{ 0x89, "ADC", 1, 4, 4, false }, // ADC A, C
		{ 0x8A, "ADC", 1, 4, 4, false }, // ADC A, D
		{ 0x8B, "ADC", 1, 4, 4, false }, // ADC A, E
		{ 0x8C, "ADC", 1, 4, 4, false }, // ADC A, H
		{ 0x8D, "ADC", 1, 4, 4, false }, // ADC A, L
		{ 0x8E, "ADC", 1, 8, 8, false }, // ADC A, (HL)
		{ 0x8F, "ADC", 1, 4, 4, false }, // ADC A, A
		{ 0x90, "SUB", 1, 4, 4, false }, // SUB A, B
		{ 0x91, "SUB", 1, 4, 4, false }, // SUB A, C
		{ 0x92, "SUB", 1, 4, 4, false }, // SUB A, D
		{ 0x93, "SUB", 1, 4, 4, false }, // SUB A, E
		{ 0x94, "SUB", 1, 4, 4, false }, // SUB A, H
		{ 0x95, "SUB", 1, 4, 4, false }, // SUB A, L
		{ 0x96, "SUB", 1, 8, 8, false }, // SUB A, (HL)
		{ 0x97, "SUB", 1, 4, 4, false }, // SUB A, A
		{ 0x98, "SBC", 1, 4, 4, false }, // SBC A, B
		{ 0x99, "SBC", 1, 4, 4, false }, // SBC A, C
		{ 0x9A, "SBC", 1, 4, 4, false }, // SBC A, D
		{ 0x9B, "SBC", 1, 4, 4, false }, // SBC A, E
		{ 0x9C, "SBC", 1, 4, 4, false }, // SBC A, H
		{ 0x9D, "SBC", 1, 4, 4, false }, // SBC A, L
		{ 0x9E, "SBC", 1, 8, 8, false }, // SBC A, (HL)
		{ 0x9F, "SBC", 1, 4, 4, false }, // SBC A, A
		{ 0xA0, "AND", 1, 4, 4, false }, // AND A, B
		{ 0xA1, "AND", 1, 4, 4, false }, // AND A, C
		{ 0xA2, "AND", 1, 4, 4, false }, // AND A, D
		{ 0xA3, "AND", 1, 4, 4, false }, // AND A, E
		{ 0xA4, "AND", 1, 4, 4, false }, // AND A, H
		{ 0xA5, "AND", 1, 4, 4, false }, // AND A, L
		{ 0xA6, "AND", 1, 8, 8, false }, // AND A, (HL)
		{ 0xA7, "AND", 1, 4, 4, false }, // AND A, A
		{ 0xA8, "XOR", 1, 4, 4, false }, // XOR A, B
		{ 0xA9, "XOR", 1, 4, 4, false }, // XOR A, C
		{ 0xAA, "XOR", 1, 4, 4, false }, // XOR A, D
		{ 0xAB, "XOR", 1, 4, 4, false }, // XOR A, E
		{ 0xAC, "XOR", 1, 4, 4, false }, // XOR A, H
		{ 0xAD, "XOR", 1, 4, 4, false }, // XOR A, L
		{ 0xAE, "XOR", 1, 8, 8, false }, // XOR A, (HL)
		{ 0xAF, "XOR", 1, 4, 4, false }, // XOR A, A
		{ 0xB0, "OR", 1, 4, 4, false }, // OR A, B
		{ 0xB1, "OR", 1, 4, 4, false }, // OR A, C
		{ 0xB2, "OR", 1, 4, 4, false }, // OR A, D
		{ 0xB3, "OR", 1, 4, 4, false }, // OR A, E
		{ 0xB4, "OR", 1, 4, 4, false }, // OR A, H
		{ 0xB5, "OR", 1, 4, 4, false }, // OR A, L
		{ 0xB6, "OR", 1, 8, 8, false }, // OR A, (HL)
		{ 0xB7, "OR", 1, 4, 4, false }, // OR A, A
		{ 0xB8, "CP", 1, 4, 4, false }, // CP A, B
		{ 0xB9, "CP", 1, 4, 4, false }, // CP A, C
		{ 0xBA, "CP", 1, 4, 4, false }, // CP A, D
		{ 0xBB, "CP", 1, 4, 4, false }, // CP A, E
		{ 0xBC, "CP", 1, 4, 4, false }, // CP A, H
		{ 0xBD, "CP", 1, 4, 4, false }, // CP A, L
		{ 0xBE, "CP", 1, 8, 8, false }, // CP A, (HL)
		{ 0xBF, "CP", 1, 4, 4, false }, // CP A, A
		{ 0xC0, "RET", 1, 20, 8, false }, // RET NZ
		{ 0xC1, "POP", 1, 12, 12, false }, // POP BC
		{ 0xC2, "JP", 3, 16, 12, false }, // JP NZ, a16
		{ 0xC3, "JP", 3, 16, 16, false }, // JP a16
		{ 0xC4, "CALL", 3, 24, 12, false }, // CALL NZ, a16
		{ 0xC5, "PUSH", 1, 16, 16, false }, // PUSH BC
		{ 0xC6, "ADD", 2, 8, 8, false }, // ADD A, n8
		{ 0xC7, "RST", 1, 16, 16, false }, // RST $00
		{ 0xC8, "RET", 1, 20, 8, false }, // RET Z
		{ 0xC9, "RET", 1, 16, 16, false }, // RET
		{ 0xCA, "JP", 3, 16, 12, false }, // JP Z, a16
		{ 0xCB, "PREFIX", 1, 4, 4, false }, // PREFIX
		{ 0xCC, "CALL", 3, 24, 12, false }, // CALL Z, a16
		{ 0xCD, "CALL", 3, 24, 24, false }, // CALL a16
		{ 0xCE, "ADC", 2, 8, 8, false }, // ADC A, n8
		{ 0xCF, "RST", 1, 16, 16, false }, // RST $08
		{ 0xD0, "RET", 1, 20, 8, false }, // RET NC
		{ 0xD1, "POP", 1, 12, 12, false }, // POP DE
		{ 0xD2, "JP", 3, 16, 12, false }, // JP NC, a16
		{ 0xD3, "ILLEGAL_D3", 1, 4, 4, true }, // ILLEGAL_D3
		{ 0xD4, "CALL", 3, 24, 12, false }, // CALL NC, a16
		{ 0xD5, "PUSH", 1, 16, 16, false }, // PUSH DE
		{ 0xD6, "SUB", 2, 8, 8, false }, // SUB A, n8
		{ 0xD7, "RST", 1, 16, 16, false }, // RST $10
		{ 0xD8, "RET", 1, 20, 8, false }, // RET C
		{ 0xD9, "RETI", 1, 16, 16, false }, // RETI
		{ 0xDA, "JP", 3, 16, 12, false }, // JP C, a16
		{ 0xDB, "ILLEGAL_DB", 1, 4, 4, true }, // ILLEGAL_DB
		{ 0xDC, "CALL", 3, 24, 12, false }, // CALL C, a16
		{ 0xDD, "ILLEGAL_DD", 1, 4, 4, true }, // ILLEGAL_DD
		{ 0xDE, "SBC", 2, 8, 8, false }, // SBC A, n8
		{ 0xDF, "RST", 1, 16, 16, false }, // RST $18
		{ 0xE0, "LDH", 2, 12, 12, false }, // LDH (a8), A
		{ 0xE1, "POP", 1, 12, 12, false }, // POP HL
		{ 0xE2, "LDH", 1, 8, 8, false }, // LDH (C), A
		{ 0xE3, "ILLEGAL_E3", 1, 4, 4, true }, // ILLEGAL_E3
		{ 0xE4, "ILLEGAL_E4", 1, 4, 4, true }, // ILLEGAL_E4
		{ 0xE5, "PUSH", 1, 16, 16, false }, // PUSH HL
		{ 0xE6, "AND", 2, 8, 8, false }, // AND A, n8
		{ 0xE7, "RST", 1, 16, 16, false }, // RST $20
		{ 0xE8, "ADD", 2, 16, 16, false }, // ADD SP, e8
		{ 0xE9, "JP", 1, 4, 4, false }, // JP HL
		{ 0xEA, "LD", 3, 16, 16, false }, // LD (a16), A
		{ 0xEB, "ILLEGAL_EB", 1, 4, 4, true }, // ILLEGAL_EB
		{ 0xEC, "ILLEGAL_EC", 1, 4, 4, true }, // ILLEGAL_EC
		{ 0xED, "ILLEGAL_ED", 1, 4, 4, true }, // ILLEGAL_ED
		{ 0xEE, "XOR", 2, 8, 8, false }, // XOR A, n8
		{ 0xEF, "RST", 1, 16, 16, false }, // RST $28
		{ 0xF0, "LDH", 2, 12, 12, false }, // LDH A, (a8)
		{ 0xF1, "POP", 1, 12, 12, false }, // POP AF
		{ 0xF2, "LDH", 1, 8, 8, false }, // LDH A, (C)
		{ 0xF3, "DI", 1, 4, 4, false }, // DI
		{ 0xF4, "ILLEGAL_F4", 1, 4, 4, true }, // ILLEGAL_F4
		{ 0xF5, "PUSH", 1, 16, 16, false }, // PUSH AF
		{ 0xF6, "OR", 2, 8, 8, false }, // OR A, n8
		{ 0xF7, "RST", 1, 16, 16, false }, // RST $30
		{ 0xF8, "LD", 2, 12, 12, false }, // LD HL, SP+, e8
		{ 0xF9, "LD", 1, 8, 8, false }, // LD SP, HL
		{ 0xFA, "LD", 3, 16, 16, false }, // LD A, (a16)
		{ 0xFB, "EI", 1, 4, 4, false }, // EI
		{ 0xFC, "ILLEGAL_FC", 1, 4, 4, true }, // ILLEGAL_FC
		{ 0xFD, "ILLEGAL_FD", 1, 4, 4, true }, // ILLEGAL_FD
		{ 0xFE, "CP", 2, 8, 8, false }, // CP A, n8
		{ 0xFF, "RST", 1, 16, 16, false }, // RST $38
	};
	
	OpcodeInfo const CBPrefixedOpcodeTable[256] =
	{
		{ 0x00, "RLC", 2, 8, 8, false }, // RLC B
		{ 0x01, "RLC", 2, 8, 8, false }, // RLC C
		{ 0x02, "RLC", 2, 8, 8, false }, // RLC D
		{ 0x03, "RLC", 2, 8, 8, false }, // RLC E
		{ 0x04, "RLC", 2, 8, 8, false }, // RLC H
		{ 0x05, "RLC", 2, 8, 8, false }, // RLC L
		{ 0x06, "RLC", 2, 16, 16, false }, // RLC (HL)
		{ 0x07, "RLC", 2, 8, 8, false }, // RLC A
		{ 0x08, "RRC", 2, 8, 8, false }, // RRC B
		{ 0x09, "RRC", 2, 8, 8, false }, // RRC C
		{ 0x0A, "RRC", 2, 8, 8, false }, // RRC D
		{ 0x0B, "RRC", 2, 8, 8, false }, // RRC E
		{ 0x0C, "RRC", 2, 8, 8, false }, // RRC H
		{ 0x0D, "RRC", 2, 8, 8, false }, // RRC L
		{ 0x0E, "RRC", 2, 16, 16, false }, // RRC (HL)
		{ 0x0F, "RRC", 2, 8, 8, false }, // RRC A
		{ 0x10, "RL", 2, 8, 8, false }, // RL B
		{ 0x11, "RL", 2, 8, 8, false }, // RL C
		{ 0x12, "RL", 2, 8, 8, false }, // RL D
		{ 0x13, "RL", 2, 8, 8, false }, // RL E
		{ 0x14, "RL", 2, 8, 8, false }, // RL H
		{ 0x15, "RL", 2, 8, 8, false }, // RL L
		{ 0x16, "RL", 2, 16, 16, false }, // RL (HL)
		{ 0x17, "RL", 2, 8, 8, false }, // RL A
		{ 0x18, "RR", 2, 8, 8, false }, // RR B
		{ 0x19, "RR", 2, 8, 8, false }, // RR C
		{ 0x1A, "RR", 2, 8, 8, false }, // RR D
		{ 0x1B, "RR", 2, 8, 8, false }, // RR E
		{ 0x1C, "RR", 2, 8, 8, false }, // RR H
		{ 0x1D, "RR", 2, 8, 8, false }, // RR L
		{ 0x1E, "RR", 2, 16, 16, false }, // RR (HL)
		{ 0x1F, "RR", 2, 8, 8, false }, // RR A
		{ 0x20, "SLA", 2, 8, 8, false }, // SLA B
		{ 0x21, "SLA", 2, 8, 8, false }, // SLA C
		{ 0x22, "SLA", 2, 8, 8, false }, // SLA D
		{ 0x23, "SLA", 2, 8, 8, false }, // SLA E
		{ 0x24, "SLA", 2, 8, 8, false }, // SLA H
		{ 0x25, "SLA", 2, 8, 8, false }, // SLA L
		{ 0x26, "SLA", 2, 16, 16, false }, // SLA (HL)
		{ 0x27, "SLA", 2, 8, 8, false }, // SLA A
		{ 0x28, "SRA", 2, 8, 8, false }, // SRA B
		{ 0x29, "SRA", 2, 8, 8, false }, // SRA C
		{ 0x2A, "SRA", 2, 8, 8, false }, // SRA D
		{ 0x2B, "SRA", 2, 8, 8, false }, // SRA E
		{ 0x2C, "SRA", 2, 8, 8, false }, // SRA H
		{ 0x2D, "SRA", 2, 8, 8, false }, // SRA L
		{ 0x2E, "SRA", 2, 16, 16, false }, // SRA (HL)
		{ 0x2F, "SRA", 2, 8, 8, false }, // SRA A
		{ 0x30, "SWAP", 2, 8, 8, false }, // SWAP B
		{ 0x31, "SWAP", 2, 8, 8, false }, // SWAP C
		{ 0x32, "SWAP", 2, 8, 8, false }, // SWAP D
		{ 0x33, "SWAP", 2, 8, 8, false }, // SWAP E
		{ 0x34, "SWAP", 2, 8, 8, false }, // SWAP H
		{ 0x35, "SWAP", 2, 8, 8, false }, // SWAP L
		{ 0x36, "SWAP", 2, 16, 16, false }, // SWAP (HL)
		{ 0x37, "SWAP", 2, 8, 8, false }, // SWAP A
		{ 0x38, "SRL", 2, 8, 8, false }, // SRL B
		{ 0x39, "SRL", 2, 8, 8, false }, // SRL C
		{ 0x3A, "SRL", 2, 8, 8, false }, // SRL D
		{ 0x3B, "SRL", 2, 8, 8, false }, // SRL E
		{ 0x3C, "SRL", 2, 8, 8, false }, // SRL H
		{ 0x3D, "SRL", 2, 8, 8, false }, // SRL L
		{ 0x3E, "SRL", 2, 16, 16, false }, // SRL (HL)
		{ 0x3F, "SRL", 2, 8, 8, false }, // SRL A
		{ 0x40, "BIT", 2, 8, 8, false }, // BIT 0, B
		{ 0x41, "BIT", 2, 8, 8, false }, // BIT 0, C
		{ 0x42, "BIT", 2, 8, 8, false }, // BIT 0, D
		{ 0x43, "BIT", 2, 8, 8, false }, // BIT 0, E
		{ 0x44, "BIT", 2, 8, 8, false }, // BIT 0, H
		{ 0x45, "BIT", 2, 8, 8, false }, // BIT 0, L
		{ 0x46, "BIT", 2, 12, 12, false }, // BIT 0, (HL)
		{ 0x47, "BIT", 2, 8, 8, false }, // BIT 0, A
		{ 0x48, "BIT", 2, 8, 8, false }, // BIT 1, B
		{ 0x49, "BIT", 2, 8, 8, false }, // BIT 1, C
		{ 0x4A, "BIT", 2, 8, 8, false }, // BIT 1, D
		{ 0x4B, "BIT", 2, 8, 8, false }, // BIT 1, E
		{ 0x4C, "BIT", 2, 8, 8, false }, // BIT 1, H
		{ 0x4D, "BIT", 2, 8, 8, false }, // BIT 1, L
		{ 0x4E, "BIT", 2, 12, 12, false }, // BIT 1, (HL)
		{ 0x4F, "BIT", 2, 8, 8, false }, // BIT 1, A
		{ 0x50, "BIT", 2, 8, 8, false }, // BIT 2, B
		{ 0x51, "BIT", 2, 8, 8, false }, // BIT 2, C
		{ 0x52, "BIT", 2, 8, 8, false }, // BIT 2, D
		{ 0x53, "BIT", 2, 8, 8, false }, // BIT 2, E
		{ 0x54, "BIT", 2, 8, 8, false }, // BIT 2, H
		{ 0x55, "BIT", 2, 8, 8, false }, // BIT 2, L
		{ 0x56, "BIT", 2, 12, 12, false }, // BIT 2, (HL)
		{ 0x57, "BIT", 2, 8, 8, false }, // BIT 2, A
		{ 0x58, "BIT", 2, 8, 8, false }, // BIT 3, B
		{ 0x59, "BIT", 2, 8, 8, false }, // BIT 3, C
		{ 0x5A, "BIT", 2, 8, 8, false }, // BIT 3, D
		{ 0x5B, "BIT", 2, 8, 8, false }, // BIT 3, E
		{ 0x5C, "BIT", 2, 8, 8, false }, // BIT 3, H
		{ 0x5D, "BIT", 2, 8, 8, false }, // BIT 3, L
		{ 0x5E, "BIT", 2, 12, 12, false }, // BIT 3, (HL)
		{ 0x5F, "BIT", 2, 8, 8, false }, // BIT 3, A
		{ 0x60, "BIT", 2, 8, 8, false }, // BIT 4, B
		{ 0x61, "BIT", 2, 8, 8, false }, // BIT 4, C
		{ 0x62, "BIT", 2, 8, 8, false }, // BIT 4, D
		{ 0x63, "BIT", 2, 8, 8, false }, // BIT 4, E
		{ 0x64, "BIT", 2, 8, 8, false }, // BIT 4, H
		{ 0x65, "BIT", 2, 8, 8, false }, // BIT 4, L
		{ 0x66, "BIT", 2, 12, 12, false }, // BIT 4, (HL)
		{ 0x67, "BIT", 2, 8, 8, false }, // BIT 4, A
		{ 0x68, "BIT", 2, 8, 8, false }, // BIT 5, B
		{ 0x69, "BIT", 2, 8, 8, false }, // BIT 5, C
		{ 0x6A, "BIT", 2, 8, 8, false }, // BIT 5, D
		{ 0x6B, "BIT", 2, 8, 8, false }, // BIT 5, E
		{ 0x6C, "BIT", 2, 8, 8, false }, // BIT 5, H
		{ 0x6D, "BIT", 2, 8, 8, false }, // BIT 5, L
		{ 0x6E, "BIT", 2, 12, 12, false }, // BIT 5, (HL)
		{ 0x6F, "BIT", 2, 8, 8, false }, // BIT 5, A
		{ 0x70, "BIT", 2, 8, 8, false }, // BIT 6, B
		{ 0x71, "BIT", 2, 8, 8, false }, // BIT 6, C
		{ 0x72, "BIT", 2, 8, 8, false }, // BIT 6, D
		{ 0x73, "BIT", 2, 8, 8, false }, // BIT 6, E
		{ 0x74, "BIT", 2, 8, 8, false }, // BIT 6, H
		{ 0x75, "BIT", 2, 8, 8, false }, // BIT 6, L
		{ 0x76, "BIT", 2, 12, 12, false }, // BIT 6, (HL)
		{ 0x77, "BIT", 2, 8, 8, false }, // BIT 6, A
		{ 0x78, "BIT", 2, 8, 8, false }, // BIT 7, B
		{ 0x79, "BIT", 2, 8, 8, false }, // BIT 7, C
		{ 0x7A, "BIT", 2, 8, 8, false }, // BIT 7, D
		{ 0x7B, "BIT", 2, 8, 8, false }, // BIT 7, E
		{ 0x7C, "BIT", 2, 8, 8, false }, // BIT 7, H
		{ 0x7D, "BIT", 2, 8, 8, false }, // BIT 7, L
		{ 0x7E, "BIT", 2, 12, 12, false }, // BIT 7, (HL)
		{ 0x7F, "BIT", 2, 8, 8, false }, // BIT 7, A
		{ 0x80, "RES", 2, 8, 8, false }, // RES 0, B
		{ 0x81, "RES", 2, 8, 8, false }, // RES 0, C
		{ 0x82, "RES", 2, 8, 8, false }, // RES 0, D
		{ 0x83, "RES", 2, 8, 8, false }, // RES 0, E
		{ 0x84, "RES", 2, 8, 8, false }, // RES 0, H
		{ 0x85, "RES", 2, 8, 8, false }, // RES 0, L
		{ 0x86, "RES", 2, 16, 16, false }, // RES 0, (HL)
		{ 0x87, "RES", 2, 8, 8, false }, // RES 0, A
		{ 0x88, "RES", 2, 8, 8, false }, // RES 1, B
		{ 0x89, "RES", 2, 8, 8, false }, // RES 1, C
		{ 0x8A, "RES", 2, 8, 8, false }, // RES 1, D
		{ 0x8B, "RES", 2, 8, 8, false }, // RES 1, E
		{ 0x8C, "RES", 2, 8, 8, false }, // RES 1, H
		{ 0x8D, "RES", 2, 8, 8, false }, // RES 1, L
		{ 0x8E, "RES", 2, 16, 16, false }, // RES 1, (HL)
		{ 0x8F, "RES", 2, 8, 8, false }, // RES 1, A
		{ 0x90, "RES", 2, 8, 8, false }, // RES 2, B
		{ 0x91, "RES", 2, 8, 8, false }, // RES 2, C
		{ 0x92, "RES", 2, 8, 8, false }, // RES 2, D
		{ 0x93, "RES", 2, 8, 8, false }, // RES 2, E
		{ 0x94, "RES", 2, 8, 8, false }, // RES 2, H
		{ 0x95, "RES", 2, 8, 8, false }, // RES 2, L
		{ 0x96, "RES", 2, 16, 16, false }, // RES 2, (HL)
		{ 0x97, "RES", 2, 8, 8, false }, // RES 2, A
		{ 0x98, "RES", 2, 8, 8, false }, // RES 3, B
		{ 0x99, "RES", 2, 8, 8, false }, // RES 3, C
		{ 0x9A, "RES", 2, 8, 8, false }, // RES 3, D
		{ 0x9B, "RES", 2, 8, 8, false }, // RES 3, E
		{ 0x9C, "RES", 2, 8, 8, false }, // RES 3, H
		{ 0x9D, "RES", 2, 8, 8, false }, // RES 3, L
		{ 0x9E, "RES", 2, 16, 16, false }, // RES 3, (HL)
		{ 0x9F, "RES", 2, 8, 8, false }, // RES 3, A
		{ 0xA0, "RES", 2, 8, 8, false }, // RES 4, B
		{ 0xA1, "RES", 2, 8, 8, false }, // RES 4, C
		{ 0xA2, "RES", 2, 8, 8, false }, // RES 4, D
		{ 0xA3, "RES", 2, 8, 8, false }, // RES 4, E
		{ 0xA4, "RES", 2, 8, 8, false }, // RES 4, H
		{ 0xA5, "RES", 2, 8, 8, false }, // RES 4, L
		{ 0xA6, "RES", 2, 16, 16, false }, // RES 4, (HL)
		{ 0xA7, "RES", 2, 8, 8, false }, // RES 4, A
		{ 0xA8, "RES", 2, 8, 8, false }, // RES 5, B
		{ 0xA9, "RES", 2, 8, 8, false }, // RES 5, C
		{ 0xAA, "RES", 2, 8, 8, false }, // RES 5, D
		{ 0xAB, "RES", 2, 8, 8, false }, // RES 5, E
		{ 0xAC, "RES", 2, 8, 8, false }, // RES 5, H
		{ 0xAD, "RES", 2, 8, 8, false }, // RES 5, L
		{ 0xAE, "RES", 2, 16, 16, false }, // RES 5, (HL)
		{ 0xAF, "RES", 2, 8, 8, false }, // RES 5, A
		{ 0xB0, "RES", 2, 8, 8, false }, // RES 6, B
		{ 0xB1, "RES", 2, 8, 8, false }, // RES 6, C
		{ 0xB2, "RES", 2, 8, 8, false }, // RES 6, D
		{ 0xB3, "RES", 2, 8, 8, false }, // RES 6, E
		{ 0xB4, "RES", 2, 8, 8, false }, // RES 6, H
		{ 0xB5, "RES", 2, 8, 8, false }, // RES 6, L
		{ 0xB6, "RES", 2, 16, 16, false }, // RES 6, (HL)
		{ 0xB7, "RES", 2, 8, 8, false }, // RES 6, A
		{ 0xB8, "RES", 2, 8, 8, false }, // RES 7, B
		{ 0xB9, "RES", 2, 8, 8, false }, // RES 7, C
		{ 0xBA, "RES", 2, 8, 8, false }, // RES 7, D
		{ 0xBB, "RES", 2, 8, 8, false }, // RES 7, E
		{ 0xBC, "RES", 2, 8, 8, false }, // RES 7, H
		{ 0xBD, "RES", 2, 8, 8, false }, // RES 7, L
		{ 0xBE, "RES", 2, 16, 16, false }, // RES 7, (HL)
		{ 0xBF, "RES", 2, 8, 8, false }, // RES 7, A
		{ 0xC0, "SET", 2, 8, 8, false }, // SET 0, B
		{ 0xC1, "SET", 2, 8, 8, false }, // SET 0, C
		{ 0xC2, "SET", 2, 8, 8, false }, // SET 0, D
		{ 0xC3, "SET", 2, 8, 8, false }, // SET 0, E
		{ 0xC4, "SET", 2, 8, 8, false }, // SET 0, H
		{ 0xC5, "SET", 2, 8, 8, false }, // SET 0, L
		{ 0xC6, "SET", 2, 16, 16, false }, // SET 0, (HL)
		{ 0xC7, "SET", 2, 8, 8, false }, // SET 0, A
		{ 0xC8, "SET", 2, 8, 8, false }, // SET 1, B
		{ 0xC9, "SET", 2, 8, 8, false }, // SET 1, C
		{ 0xCA, "SET", 2, 8, 8, false }, // SET 1, D
		{ 0xCB, "SET", 2, 8, 8, false }, // SET 1, E
		{ 0xCC, "SET", 2, 8, 8, false }, // SET 1, H
		{ 0xCD, "SET", 2, 8, 8, false }, // SET 1, L
		{ 0xCE, "SET", 2, 16, 16, false }, // SET 1, (HL)
		{ 0xCF, "SET", 2, 8, 8, false }, // SET 1, A
		{ 0xD0, "SET", 2, 8, 8, false }, // SET 2, B
		{ 0xD1, "SET", 2, 8, 8, false }, // SET 2, C
		{ 0xD2, "SET", 2, 8, 8, false }, // SET 2, D
		{ 0xD3, "SET", 2, 8, 8, false }, // SET 2, E
		{ 0xD4, "SET", 2, 8, 8, false }, // SET 2, H
		{ 0xD5, "SET", 2, 8, 8, false }, // SET 2, L
		{ 0xD6, "SET", 2, 16, 16, false }, // SET 2, (HL)
		{ 0xD7, "SET", 2, 8, 8, false }, // SET 2, A
		{ 0xD8, "SET", 2, 8, 8, false }, // SET 3, B
		{ 0xD9, "SET", 2, 8, 8, false }, // SET 3, C
		{ 0xDA, "SET", 2, 8, 8, false }, // SET 3, D
		{ 0xDB, "SET", 2, 8, 8, false }, // SET 3, E
		{ 0xDC, "SET", 2, 8, 8, false }, // SET 3, H
		{ 0xDD, "SET", 2, 8, 8, false }, // SET 3, L
		{ 0xDE, "SET", 2, 16, 16, false }, // SET 3, (HL)
		{ 0xDF, "SET", 2, 8, 8, false }, // SET 3, A
		{ 0xE0, "SET", 2, 8, 8, false }, // SET 4, B
		{ 0xE1, "SET", 2, 8, 8, false }, // SET 4, C
		{ 0xE2, "SET", 2, 8, 8, false }, // SET 4, D
		{ 0xE3, "SET", 2, 8, 8, false }, // SET 4, E
		{ 0xE4, "SET", 2, 8, 8, false }, // SET 4, H
		{ 0xE5, "SET", 2, 8, 8, false }, // SET 4, L
		{ 0xE6, "SET", 2, 16, 16, false }, // SET 4, (HL)
		{ 0xE7, "SET", 2, 8, 8, false }, // SET 4, A
		{ 0xE8, "SET", 2, 8, 8, false }, // SET 5, B
		{ 0xE9, "SET", 2, 8, 8, false }, // SET 5, C
		{ 0xEA, "SET", 2, 8, 8, false }, // SET 5, D
		{ 0xEB, "SET", 2, 8, 8, false }, // SET 5, E
		{ 0xEC, "SET", 2, 8, 8, false }, // SET 5, H
		{ 0xED, "SET", 2, 8, 8, false }, // SET 5, L
		{ 0xEE, "SET", 2, 16, 16, false }, // SET 5, (HL)
		{ 0xEF, "SET", 2, 8, 8, false }, // SET 5, A
		{ 0xF0, "SET", 2, 8, 8, false }, // SET 6, B
		{ 0xF1, "SET", 2, 8, 8, false }, // SET 6, C
		{ 0xF2, "SET", 2, 8, 8, false }, // SET 6, D
		{ 0xF3, "SET", 2, 8, 8, false }, // SET 6, E
		{ 0xF4, "SET", 2, 8, 8, false }, // SET 6, H
		{ 0xF5, "SET", 2, 8, 8, false }, // SET 6, L
		{ 0xF6, "SET", 2, 16, 16, false }, // SET 6, (HL)
		{ 0xF7, "SET", 2, 8, 8, false }, // SET 6, A
		{ 0xF8, "SET", 2, 8, 8, false }, // SET 7, B
		{ 0xF9, "SET", 2, 8, 8, false }, // SET 7, C
		{ 0xFA, "SET", 2, 8, 8, false }, // SET 7, D
		{ 0xFB, "SET", 2, 8, 8, false }, // SET 7, E
		{ 0xFC, "SET", 2, 8, 8, false }, // SET 7, H
		{ 0xFD, "SET", 2, 8, 8, false }, // SET 7, L
		{ 0xFE, "SET", 2, 16, 16, false }, // SET 7, (HL)
		{ 0xFF, "SET", 2, 8, 8, false }, // SET 7, A
	};
}
