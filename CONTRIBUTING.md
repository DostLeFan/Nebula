# Contributing to Nebula

First of all, thanks for your interest in contributing!

Nebula is still in early development. The core is evolving quickly, so contributions are very welcome, especially in the following areas:

- Bug reports and reproductions
- Unit tests
- Documentation
- New Memory Bank Controllers (if relevant)
- Help on other features of the project (all of the core, for the moment)
- Performance and correctness improvements
- Bindings for other languages

---


## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Before You Start](#before-you-start)
- [Getting Started](#getting-started)
- [Building & Testing](#building--testing)
- [Code Style](#code-style)
- [Commit Messages](#commit-messages)
- [Pull Requests](#pull-requests)
- [Submitting a Contribution](#submitting-a-contribution)
- [Reporting Bugs](#reporting-bugs)
- [Suggesting Features](#suggesting-features)

---

## Code of Conduct

Be respectful. Constructive criticism is welcome; personal attacks are not.

---

## Before You Start

New to the project? Look for issues labeled good first issue - they are scoped, self-contained, and a good way to get familiar with the codebase.

- For small fixes (typos, minor bugs, documentation): feel free to open a PR directly.
- For larger features or architectural changes: please open an issue or a discussion first. This avoids wasted effort if the direction doesn't fit the project.
- Check the existing issues and PRs to avoid duplicates.

---

## Getting Started

1. Fork the repository
2. Clone your fork
3. Create a new branch (`git checkout -b feature/my-feature`)
4. Build the project (see below)
5. Make your changes
6. Make sure the tests pass
7. Open a Pull Request

---

## Building & Testing

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DNEBULA_BUILD_TEST=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

You can also use the provided CMake presets:

```bash
cmake --preset release
cmake --build --preset release
```

---

## Code Style

- C++17
- Prefer clarity over cleverness
- Follow the existing style in the codebase as closely as possible
- Use meaningful names
- Keep functions focused

We do not enforce a strict formatter yet (clang-format will probably be added later).

---

## Commit Messages

Use clear and descriptive commit messages.

Conventional Commits style is appreciated but not mandatory:

- `feat: add MBC7 support`
- `fix: correct VRAM bank switching`
- `test: add coverage for scheduler edge cases`
- `docs: improve building instructions`

---

## Pull Requests

- Keep PRs focused (one topic per PR when possible)
- Make sure CI passes
- Describe what the PR does and why
- Link related issues if any

---

## Submitting a Contribution

1. **Fork** the repository and create a branch from `main`:
   ```bash
   git checkout -b feature/my-feature
   ```

2. **Make your changes.** Keep commits focused and write clear commit messages.

3. **Test your changes locally** before submitting.

4. **Open a Pull Request** against `main`. In the PR description, explain:
   - What the change does
   - Why it is needed
   - How you tested it

5. Be ready for feedback. PRs may require changes before being merged.

---

## Reporting Bugs

Open an issue and include:

- Your OS and compiler version
- The exact command you ran
- The expected vs. actual behavior
- If possible, a minimal reproducer if relevant

---

## Suggesting Features

Open an issue with the `enhancement` label. Describe:

- The problem you are trying to solve
- Your proposed solution
- Any alternatives you considered

For large changes, a discussion is preferred over a direct issue.

---

## Questions?

Feel free to open an issue for discussion before starting large changes.