/*
	nebula_opcode_generator
	
	Reads the gbdev/gb-opcodes reference JSON (https://github.com/gbdev/gb-opcodes,
	mirrored at https://gbdev.io/gb-opcodes/Opcodes.json) and generates the static
	OpcodeInfo tables consumed by the CPU (see core/include/nebula/CPU/OpcodeInfo.hpp).
	
	This tool only produces basic, descriptive metadata (mnemonic, instruction length,
	timing, legality) : it does NOT generate any execution logic. The semantics of each
	opcode are implemented by hand in the CPU, keyed by opcode byte.
	
	Usage:
		nebula_opcode_generator <path-to-Opcodes.json> <path-to-output.cpp>
	
	This is normally invoked through the `regen_opcodes` CMake target (see
	tools/opcode-gen/CMakeLists.txt and README.md), not run directly.
*/

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <array>
#include <stdexcept>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace
{
	// One fully-resolved table entry, ready to be printed as a C++ initializer.
	struct Entry
	{
		uint8_t opcode;
		std::string mnemonic;
		uint8_t bytes;
		uint8_t tCyclesTaken;
		uint8_t tCyclesNotTaken;
		bool illegal;
		std::string operandsComment; // Human-readable operand list, for the generated comment only.
	};
	
	// Builds the "0x00".."0xFF" uppercase-hex key used by the reference JSON.
	std::string hexKey(unsigned value)
	{
		char buf[8];
		std::snprintf(buf, sizeof(buf), "0x%02X", value);
		return std::string(buf);
	}
	
	// Human-readable operand list for the trailing comment (e.g. "BC, n16" or "(HL)+, A").
	std::string describeOperands(json const& operands)
	{
		std::string result;
		
		for(size_t i=0;i<operands.size();++i)
		{
			json const& op = operands[i];
			std::string name = op.at("name").get<std::string>();
			bool immediate = op.value("immediate", true);
			bool increment = op.value("increment", false);
			bool decrement = op.value("decrement", false);
			
			std::string piece = immediate ? name : ("(" + name + ")");
			
			if(increment)
				piece += "+";
			else if(decrement)
				piece += "-";
			
			if(i > 0)
				result += ", ";
			
			result += piece;
		}
		
		return result;
	}
	
	// Parses one 256-entry section ("unprefixed" or "cbprefixed") of the reference JSON.
	std::array<Entry, 256> parseSection(json const& section, char const* sectionName)
	{
		std::array<Entry, 256> table{};
		
		for(unsigned opcode=0;opcode<256;++opcode)
		{
			std::string key = hexKey(opcode);
			
			if(!section.contains(key))
				throw std::runtime_error(std::string("Missing opcode ") + key + " in section \"" + sectionName + "\"");
			
			json const& entry = section.at(key);
			
			Entry parsed{};
			parsed.opcode = static_cast<uint8_t>(opcode);
			parsed.mnemonic = entry.at("mnemonic").get<std::string>();
			parsed.bytes = entry.at("bytes").get<uint8_t>();
			
			json const& cycles = entry.at("cycles");
			
			if(cycles.empty())
				throw std::runtime_error(std::string("Opcode ") + key + " (" + sectionName + ") has an empty \"cycles\" array");
			
			parsed.tCyclesTaken = cycles[0].get<uint8_t>();
			parsed.tCyclesNotTaken = (cycles.size() > 1) ? cycles[1].get<uint8_t>() : parsed.tCyclesTaken;
			
			// Illegal opcodes are named "ILLEGAL_XX" in the reference JSON.
			parsed.illegal = (parsed.mnemonic.rfind("ILLEGAL_", 0) == 0);
			
			parsed.operandsComment = describeOperands(entry.at("operands"));
			
			table[opcode] = parsed;
		}
		
		return table;
	}
	
	// Emits one 256-entry C++ array definition.
	void writeTable(std::ostream& os, std::string const& arrayName, std::array<Entry, 256> const& table)
	{
		os << "\tOpcodeInfo const " << arrayName << "[256] =\n\t{\n";
		
		for(Entry const& e : table)
		{
			os << "\t\t{ 0x" << std::hex << std::uppercase;
			
			// Manual hex formatting (avoids leaking stream flags into later fields).
			char opcodeBuf[3];
			std::snprintf(opcodeBuf, sizeof(opcodeBuf), "%02X", e.opcode);
			os << opcodeBuf << std::dec;
			
			os << ", \"" << e.mnemonic << "\", "
			   << static_cast<unsigned>(e.bytes) << ", "
			   << static_cast<unsigned>(e.tCyclesTaken) << ", "
			   << static_cast<unsigned>(e.tCyclesNotTaken) << ", "
			   << (e.illegal ? "true" : "false")
			   << " }, // " << e.mnemonic;
			
			if(!e.operandsComment.empty())
				os << " " << e.operandsComment;
			
			os << "\n";
		}
		
		os << "\t};\n";
	}
}

int main(int argc, char** argv)
{
	if(argc != 3)
	{
		std::fprintf(stderr, "Usage: %s <path-to-Opcodes.json> <path-to-output.cpp>\n", argv[0]);
		return 1;
	}
	
	std::string const jsonPath = argv[1];
	std::string const outputPath = argv[2];
	
	try
	{
		std::ifstream jsonFile(jsonPath);
		
		if(!jsonFile)
			throw std::runtime_error("Could not open input JSON file: " + jsonPath);
		
		json root;
		jsonFile >> root;
		
		std::array<Entry, 256> unprefixed = parseSection(root.at("unprefixed"), "unprefixed");
		std::array<Entry, 256> cbPrefixed = parseSection(root.at("cbprefixed"), "cbprefixed");
		
		std::ofstream out(outputPath, std::ios::trunc);
		
		if(!out)
			throw std::runtime_error("Could not open output file for writing: " + outputPath);
		
		out << "// GENERATED FILE - DO NOT EDIT BY HAND.\n";
		out << "// Regenerate with the `regen_opcodes` CMake target (see tools/opcode-gen).\n";
		out << "// Source: https://github.com/gbdev/gb-opcodes (Opcodes.json)\n\n";
		out << "#include \"../../../include/nebula/CPU/OpcodeInfo.hpp\"\n\n";
		out << "namespace nebula\n{\n";
		
		writeTable(out, "UnprefixedOpcodeTable", unprefixed);
		out << "\t\n";
		writeTable(out, "CBPrefixedOpcodeTable", cbPrefixed);
		
		out << "}\n";
		
		std::printf("Generated %s (256 unprefixed + 256 CB-prefixed opcodes).\n", outputPath.c_str());
	}
	catch(std::exception const& e)
	{
		std::fprintf(stderr, "nebula_opcode_generator: error: %s\n", e.what());
		return 1;
	}
	
	return 0;
}
