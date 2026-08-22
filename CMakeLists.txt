cmake_minimum_required(VERSION 3.25)

project(nebula VERSION 0.0.8 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(NEBULA_LIB_NAME nebula-core)
set(NEBULA_EMULATOR_NAME nebula)
set(NEBULA_NODE_MODULE_NAME nebula)
set(NEBULA_TESTS_NAME nebula_tests)
set(NEBULA_OPCODE_GENERATOR_NAME nebula_opcode_generator)

include(GNUInstallDirs)

option(NEBULA_BUILD_EMULATOR_APP "Build emulator executable" ON)
option(NEBULA_BUILD_NODE "Build NodeJS bindings" OFF)
option(NEBULA_BUILD_TEST "Build unit tests" OFF)
option(NEBULA_REGEN_OPCODES "Build the CPU opcode table generator" OFF)

add_subdirectory(core)

if(NEBULA_REGEN_OPCODES)
	add_subdirectory(tools/OpcodeGenerator)
endif()

if(NEBULA_BUILD_EMULATOR_APP)
	add_subdirectory(emulator)
endif()

if(NEBULA_BUILD_NODE)
	if(WIN32)
		set(NPM_CMD "npm.cmd")
		set(NPX_CMD "npx.cmd")
	else()
		set(NPM_CMD "npm")
		set(NPX_CMD "npx")
	endif()
	
	set(NODE_BINDING_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bindings/node")
	
	if(NOT EXISTS "${NODE_BINDING_DIR}/node_modules")
		message(STATUS "Installing local NodeJS dependencies for binding...")
		
		execute_process(
			COMMAND ${NPM_CMD} install
			WORKING_DIRECTORY ${NODE_BINDING_DIR}
		)
		
		message(STATUS "Local NodeJS dependencies for binding installed.")
	endif()
	
	set(CMAKE_JS_ARGS "")
	set(CURRENT_GLOBAL_BUILD_DIR "${CMAKE_BINARY_DIR}")
	
	# Multi-config generators (Visual Studio, Xcode) automatically append a
	# per-config subfolder (Release/, Debug/...) to CMAKE_ARCHIVE_OUTPUT_DIRECTORY.
	# Single-config generators (Makefiles, Ninja) don't. Account for that here so
	# the node binding's find_library(NEBULA_LIB_DIR) actually finds nebula-core.
	get_property(NEBULA_IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
	if(NEBULA_IS_MULTI_CONFIG)
		set(NEBULA_STATIC_LIB_DIR "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/$<CONFIG>")
	else()
		set(NEBULA_STATIC_LIB_DIR "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
	endif()
	
	if(WIN32)
		if(CMAKE_GENERATOR MATCHES "MinGW Makefiles" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
			list(APPEND CMAKE_JS_ARGS "--g++" "-G" "MinGW Makefiles")
			list(APPEND CMAKE_JS_ARGS "--CDNEBULA_GLOBAL_BUILD_DIR=${CURRENT_GLOBAL_BUILD_DIR}" "--CDNEBULA_LIB_DIR=${NEBULA_STATIC_LIB_DIR}")
		elseif(CMAKE_GENERATOR MATCHES "Visual Studio" OR CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
			list(APPEND CMAKE_JS_ARGS "--msvs" "--CDNEBULA_GLOBAL_BUILD_DIR=${CURRENT_GLOBAL_BUILD_DIR}" "--CDNEBULA_LIB_DIR=${NEBULA_STATIC_LIB_DIR}")
		endif()
	else()
		list(APPEND CMAKE_JS_ARGS "--CDNEBULA_GLOBAL_BUILD_DIR=${CURRENT_GLOBAL_BUILD_DIR}" "--CDNEBULA_LIB_DIR=${NEBULA_STATIC_LIB_DIR}")
	endif()
	
	string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_JS_BUILD_TYPE)
	
	if(NOT CMAKE_JS_BUILD_TYPE)
		set(CMAKE_JS_BUILD_TYPE "release")
	endif()
	
	list(APPEND CMAKE_JS_ARGS "--config" "${CMAKE_JS_BUILD_TYPE}")
	
	add_custom_target(build_node_binding ALL
		COMMAND ${CMAKE_COMMAND} -E env "CMAKE_SHARED_LINKER_FLAGS=" ${NPX_CMD} cmake-js compile ${CMAKE_JS_ARGS}
		WORKING_DIRECTORY ${NODE_BINDING_DIR}
		COMMENT "Compiling Node.js module via cmake-js..."
		VERBATIM
	)
	
	add_dependencies(build_node_binding ${NEBULA_LIB_NAME})
endif()

if(NEBULA_BUILD_TEST)
	enable_testing()
	add_subdirectory(tests)
endif()

# CPack/CMakePackageConfigHelpers
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/nebulaConfigVersion.cmake"
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/nebulaConfig.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/nebulaConfig.cmake"
	INSTALL_DESTINATION
		${CMAKE_INSTALL_LIBDIR}/cmake/nebula-core
)

install(
	FILES
		"${CMAKE_CURRENT_BINARY_DIR}/nebulaConfig.cmake"
		"${CMAKE_CURRENT_BINARY_DIR}/nebulaConfigVersion.cmake"
	DESTINATION
		${CMAKE_INSTALL_LIBDIR}/cmake/nebula-core
)