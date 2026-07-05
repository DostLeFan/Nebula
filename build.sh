#!/bin/bash
PRESET=${1:-debug}
GENERATOR=${2:-"Unix Makefiles"}

cmake --preset "$PRESET" -G "$GENERATOR"
cmake --build --preset "$PRESET"