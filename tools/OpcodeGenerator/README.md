# Nebula OpcodeGenerator

Generates the static `OpcodeInfo` tables used by the CPU (`UnprefixedOpcodeTable` and `CBPrefixedOpcodeTable`, declared
in `core/include/nebula/CPU/OpcodeInfo.hpp`), from the [gbdev/gb-opcodes](https://github.com/gbdev/gb-opcodes) reference JSON
(mirrored at https://gbdev.io/gb-opcodes/Opcodes.json).

This tool only produces **descriptive metadata**: mnemonic, instruction length in bytes, timing in T-cycles (branch taken / not taken), and whether the opcode
is one of the 11 illegal/undefined ones. It does **not** generate any execution logic - opcode semantics are implemented by hand elsewhere in the CPU.

## Why this isn't part of the normal build

The generated file (`core/src/nebula/CPU/OpcodeTable.generated.cpp`) is **committed to the repository**, like any other source file. Regenerating it requires
network access (to fetch the reference JSON) and is only needed when:
- Bootstrapping the table for the first time.
- The upstream reference table gets a correction (wrong mnemonic, timing fix, etc.).

For everything else - day-to-day builds, CI, contributors without network access to `gbdev.io` - the committed file is used as-is, exactly like any other source
file. This keeps normal builds offline-friendly and reproducible.

## Usage

From the repository root:

```bash
cmake -B build -DNEBULA_REGEN_OPCODES=ON
cmake --build build --target regen_opcodes
```

This downloads the latest `Opcodes.json`, builds `nebula_opcode_generator`, runs it, and overwrites `core/src/nebula/CPU/OpcodeTable.generated.cpp` in place.

**Always review the resulting diff before committing.** A change here should be intentional (e.g. "upstream fixed the timing of `SET n,(HL)`"), not a silent side
effect of re-running the tool.

## Standalone usage

The tool can also be run directly, against any locally saved copy of the JSON:

```bash
nebula_opcode_generator path/to/Opcodes.json path/to/output.cpp
```