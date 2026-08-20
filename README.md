# Nebula

[![Development](https://img.shields.io/badge/development-early-orange)](#project-status--roadmap)
[![Release](https://img.shields.io/github/v/release/DostLeFan/Nebula?include_prereleases&label=version)](https://github.com/DostLeFan/Nebula/releases)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![CMake](https://img.shields.io/badge/CMake-3.25+-blue.svg)](https://cmake.org/)
[![Tests (Push)](https://github.com/DostLeFan/Nebula/actions/workflows/tests-push.yml/badge.svg)](https://github.com/DostLeFan/Nebula/actions/workflows/tests-push.yml)
[![Tests (Release)](https://github.com/DostLeFan/Nebula/actions/workflows/tests-release.yml/badge.svg)](https://github.com/DostLeFan/Nebula/actions/workflows/tests-release.yml)
[<image-card alt="Discord" src="https://img.shields.io/discord/1538441690350751788?color=7289DA&label=Discord&logo=discord&logoColor=white" ></image-card>](https://discord.gg/BfX67ruws5)

**Nebula** (*Nintendo Emulation Bridge for Universal Logic Architecture*) is a modern, cross-platform Game Boy / Game Boy Color emulator written in pure C++17.

It is designed **first and foremost as a reusable library**.  
A ready-to-use emulator frontend will also be provided.

---

# Summary

* [Why Nebula?](#why-nebula)
* [Features](#features)
	* [Currently implemented](#currently-implemented)
	* [Coming next](#coming-next)
* [Quick Start](#quick-start)
	* [Using the standalone emulator](#using-the-standalone-emulator)
	* [Using the C++ library](#using-the-c-library)
	* [Using the Node.js bindings](#using-the-nodejs-bindings)
	* [Building from source](#building-from-source)
* [Documentation](#documentation)
* [Project Status & Roadmap](#project-status--roadmap)
* [Contributing](#contributing)
* [License](#license)

---

# Why Nebula?

Most Game Boy emulators are monolithic applications. Nebula takes a different approach:

- **Library-first** - The core is a clean, embeddable library. You can use it in your own projects with minimal friction.
- **Modern C++17** - No legacy baggage, no unnecessary dependencies.
- **Truly cross-platform & cross-compiler** - Windows, Linux, macOS, BSD... and GCC, Clang, MSVC, MinGW...
- **First-class bindings** - Official Node.js bindings are already available. More languages are planned.
- **Focus on correctness and maintainability** - Clean architecture, solid testing, and readable code.

Whether you want to build a full emulator, a tool, a research project, or just experiment, Nebula aims to be a reliable foundation.

---

# Features

## Currently implemented

- Cartridge loading and ROM header parsing
- Memory Bank Controllers: NoMBC, MBC1, MBC2, MBC3, MBC5
- Full memory bus (WRAM, VRAM, OAM, HRAM, I/O, etc.)
- Cycle-based scheduler foundation

## Coming next

- CPU (Sharp LR35902)
- PPU (Pixel Processing Unit)
- APU (Audio Processing Unit)
- Input, timers, interrupts, serial...
- Game Boy Color support
- Save states, debugging facilities, and more

---

# Quick Start

## Using the standalone emulator

The official frontend is not ready yet.  
It will be documented here as soon as it becomes usable.

## Using the C++ library

The public API is still evolving.  
High-level usage examples will be added once the core is more complete.

In the meantime, the unit tests in the `tests/` directory are the best reference for how the current components work.

## Using the Node.js bindings

Node.js binding isn't really implemented. So... It is not really usable.

```bash
npm install nebula
```

Prebuilt binaries are provided for the most common platforms (Windows x64, Linux x64/ARM64, macOS x64/ARM64). No compilation required in most cases.

```js
const nebula = require("nebula");

// API still under construction
```

## Building from source

Requirements:
- CMake ≥ 3.25
- A C++17 compiler (GCC, Clang, MSVC, MinGW...)
- (Optional) Node.js ≥ 18 if you want to build the Node bindings

```bash
git clone https://github.com/DostLeFan/nebula.git
cd nebula
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Useful CMake options:
- `NEBULA_BUILD_EMULATOR` - Build the standalone emulator
- `NEBULA_BUILD_NODE` - Build the Node.js bindings
- `NEBULA_BUILD_TEST` - Build the test suite

Full build instructions for all platforms and compilers will be available in `docs/building.md`.

---

# Documentation

- Architecture overview *(coming soon)*
- Building from source *(coming soon)*
- API reference (Doxygen - planned)
- Node.js bindings

---

# Project Status & Roadmap

Nebula is in early development (version 0.x).

**Current focus:** Solid foundation (cartridge, memory, scheduler).

**Planned order of major components:**
1. CPU (LR35902)
2. PPU
3. APU
4. Remaining hardware (timers, interrupts, input, serial...)
5. Game Boy Color mode
6. Higher-level features (save states, debugger, etc.)

The long-term goal is a highly accurate and maintainable emulator core that can be reused in many different contexts.

---

# Contributing

Contributions are welcome!

Please read CONTRIBUTING.md for guidelines (coding style, testing, pull request process, etc.).

---

# License

This project is released under the **MIT License**.

See LICENSE for the full text.