param(
	[string]$Preset = "debug",
	[string]$Generator = "MinGW Makefiles"
)

cmake --preset $Preset -G $Generator
cmake --build --preset $Preset