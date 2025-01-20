cmake_minimum_required(VERSION 3.12)

set(PROJECTS_PATH ${CMAKE_SOURCE_DIR}/..)

set(PICO_SDK_VERSION 2.0.0)
set(PICO_SDK_PATH ${PROJECTS_PATH}/pico-sdk/${PICO_SDK_VERSION})
set(PICOTOOL_FETCH_FROM_GIT_PATH ${PROJECTS_PATH}/picotool)

set(PICO_BOARD pico)
# If the board is unknown to the SDK, tell it where to find the board definition:
# set(PICO_BOARD_HEADER_DIRS <some-path-goes-here>)

set(PICO_PLATFORM rp2040)

message(STATUS "Building for ARM")

set(PICO_ARM_TOOLCHAIN_PATH   ${CMAKE_SOURCE_DIR}/cmake/toolchains/arm-none-eabi.cmake)
set(CMAKE_TOOLCHAIN_FILE ${PICO_ARM_TOOLCHAIN_PATH})

# The toolchain file explained where to find the cross-compiler.
# We need to tell that cross-compiler a bit more about how to compile the code for this project.
set(CPU cortex-m0)

set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -mcpu=${CPU} -mthumb")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=${CPU} -mthumb")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -mcpu=${CPU} -mthumb")

# This is required to avoid a linker error: undefined reference to '__cxa_guard_acquire'
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-threadsafe-statics")

# Disable C++ exception processing
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")

# Turn off RTTI since we have our own printf mechanisms
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
# This is probably the wrong way to do this...
# For now, compile all the Cortex code with -Og for optimized, but debuggable.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Og")
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Og")


# This include needs to happen before project()
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(rom-reader
  LANGUAGES C CXX
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "2.0.0")
  message(FATAL_ERROR "Raspberry Pi Pico SDK version 2.0.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()



# If you want debug output from USB (pass -DPICO_STDIO_USB=1) this ensures you don't lose any debug output while USB is set up
if (NOT DEFINED PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS)
    set(PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS 3000)
endif()

# Initialize the SDK
pico_sdk_init()

add_compile_options(
  -Wall
  -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
  -Wno-unused-function # we have some for the docs that aren't called
)

if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
  add_compile_options(-Wno-maybe-uninitialized)
endif()

add_executable(rom-reader
  rom-reader.cpp
)

# pull in common dependencies
target_link_libraries(rom-reader pico_stdlib)

if (PICO_CYW43_SUPPORTED)
  target_link_libraries(rom-reader pico_cyw43_arch_none)
endif()

# create map/bin/hex file etc.
pico_add_extra_outputs(rom-reader)
