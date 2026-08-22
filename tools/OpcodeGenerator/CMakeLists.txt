cmake_minimum_required(VERSION 3.25)

include(FetchContent)

# nlohmann/json
message(STATUS "Downloading nlohmann/json...")

FetchContent_Declare(
	nlohmann_json
	GIT_REPOSITORY https://github.com/nlohmann/json.git
	GIT_TAG v3.11.3
)

FetchContent_MakeAvailable(nlohmann_json)

message(STATUS "nlohmann/json downloaded.")

# The gbdev/gb-opcodes reference table itself.
set(NEBULA_OPCODES_JSON "${CMAKE_CURRENT_BINARY_DIR}/Opcodes.json")

message(STATUS "Downloading gbdev/gb-opcodes reference table...")

file(
	DOWNLOAD
		"https://gbdev.io/gb-opcodes/Opcodes.json"
		"${NEBULA_OPCODES_JSON}"
	STATUS NEBULA_OPCODES_DOWNLOAD_STATUS
	TLS_VERIFY ON
)

list(GET NEBULA_OPCODES_DOWNLOAD_STATUS 0 NEBULA_OPCODES_DOWNLOAD_CODE)

if(NOT NEBULA_OPCODES_DOWNLOAD_CODE EQUAL 0)
	list(GET NEBULA_OPCODES_DOWNLOAD_STATUS 1 NEBULA_OPCODES_DOWNLOAD_MESSAGE)
	message(FATAL_ERROR "Failed to download Opcodes.json: ${NEBULA_OPCODES_DOWNLOAD_MESSAGE}")
endif()

message(STATUS "gbdev/gb-opcodes reference table downloaded.")

# The generator tool itself.

add_executable(${NEBULA_OPCODE_GENERATOR_NAME} src/main.cpp)

target_link_libraries(${NEBULA_OPCODE_GENERATOR_NAME} PRIVATE nlohmann_json::nlohmann_json)

set_target_properties(${NEBULA_OPCODE_GENERATOR_NAME} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

# `cmake --build <dir> --target regen_opcodes` : runs the generator and overwrites the
# committed generated file in-place. Review the resulting diff before committing it.
set(NEBULA_GENERATED_OPCODE_TABLE "${CMAKE_SOURCE_DIR}/core/src/nebula/CPU/OpcodeTable.generated.cpp")

add_custom_target(regen_opcodes
	COMMAND ${NEBULA_OPCODE_GENERATOR_NAME} "${NEBULA_OPCODES_JSON}" "${NEBULA_GENERATED_OPCODE_TABLE}"
	DEPENDS ${NEBULA_OPCODE_GENERATOR_NAME}
	COMMENT "Regenerating ${NEBULA_GENERATED_OPCODE_TABLE} from gbdev/gb-opcodes..."
	VERBATIM
)