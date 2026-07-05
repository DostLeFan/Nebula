cmake_minimum_required(VERSION 3.25)
project(nebula_node LANGUAGES CXX)

if(NOT NEBULA_NODE_MODULE_NAME)
	set(NEBULA_NODE_MODULE_NAME nebula)
endif()

if(WIN32 AND CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	string(REPLACE "/DELAYLOAD:NODE.EXE" "" CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}")
	string(REPLACE "/DELAYLOAD:node.exe" "" CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}" CACHE STRING "" FORCE)
	
	if(CMAKE_JS_LIB)
		file(TO_CMAKE_PATH "${CMAKE_JS_LIB}" CLEAN_CMAKE_JS_LIB)
		set(CMAKE_JS_LIB "${CLEAN_CMAKE_JS_LIB}" CACHE STRING "" FORCE)
	endif()
endif()

include(FetchContent)

# node-addon-api
message(STATUS "Downloading node-addon-api...")

FetchContent_Declare(
	node-addon-api
	GIT_REPOSITORY https://github.com/nodejs/node-addon-api.git
	GIT_TAG v8.5.0
)

FetchContent_MakeAvailable(node-addon-api)

message(STATUS "node-addon-api downloaded.")

add_library(${NEBULA_NODE_MODULE_NAME} SHARED src/addon.cpp)

target_include_directories(${NEBULA_NODE_MODULE_NAME} PRIVATE ${CMAKE_JS_INC} "${node-addon-api_SOURCE_DIR}")

if(NEBULA_LIB_DIR)
	find_library(NEBULA_STATIC_LIB
		NAMES nebula-core libnebula-core
		PATHS "${NEBULA_LIB_DIR}"
		NO_DEFAULT_PATH
	)
	
	if(NEBULA_STATIC_LIB)
		message(STATUS "Library nebula found at: ${NEBULA_STATIC_LIB}")
		target_link_libraries(${NEBULA_NODE_MODULE_NAME} PRIVATE "${NEBULA_STATIC_LIB}")
	else()
		target_link_libraries(${NEBULA_NODE_MODULE_NAME} PRIVATE nebula-core)
	endif()
else()
	target_link_libraries(${NEBULA_NODE_MODULE_NAME} PRIVATE nebula-core)
endif()

if(CMAKE_JS_LIB AND EXISTS "${CMAKE_JS_LIB}")
	target_link_libraries(${NEBULA_NODE_MODULE_NAME} PRIVATE "${CMAKE_JS_LIB}")
endif()

target_compile_definitions(${NEBULA_NODE_MODULE_NAME} PRIVATE NAPI_CPP_EXCEPTIONS)

set_target_properties(${NEBULA_NODE_MODULE_NAME} PROPERTIES
	OUTPUT_NAME "nebula"
	PREFIX ""
	SUFFIX ".node"
)

if(NEBULA_GLOBAL_BUILD_DIR)
	set(FINAL_PREBUILD_DIR "${NEBULA_GLOBAL_BUILD_DIR}/prebuilds")
else()
	set(FINAL_PREBUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/prebuilds")
endif()

add_custom_command(TARGET ${NEBULA_NODE_MODULE_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory "${FINAL_PREBUILD_DIR}"
	COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${NEBULA_NODE_MODULE_NAME}>" "${FINAL_PREBUILD_DIR}/nebula.node"
	COMMENT "Secure copy of native module into: ${FINAL_PREBUILD_DIR}/nebula.node"
)