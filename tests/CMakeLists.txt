cmake_minimum_required(VERSION 3.25)

include(FetchContent)

# Catch2
message(STATUS "Downloading Catch2...")

FetchContent_Declare(
	Catch2
	GIT_REPOSITORY https://github.com/catchorg/Catch2.git
	GIT_TAG v3.5.2
)

FetchContent_MakeAvailable(Catch2)

message(STATUS "Catch2 downloaded.")

# List sources.

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(${NEBULA_TESTS_NAME} ${TEST_SOURCES})

target_link_libraries(${NEBULA_TESTS_NAME} PRIVATE ${NEBULA_LIB_NAME} Catch2::Catch2WithMain)
target_include_directories(${NEBULA_TESTS_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_test(NAME ${NEBULA_TESTS_NAME} COMMAND ${NEBULA_TESTS_NAME})

set_target_properties(${NEBULA_TESTS_NAME} PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
)

include(Catch)

catch_discover_tests(${NEBULA_TESTS_NAME})