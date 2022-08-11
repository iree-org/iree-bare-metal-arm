# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

cmake_minimum_required(VERSION 3.13.4)

#-------------------------------------------------------------------------------
# Project configuration
#-------------------------------------------------------------------------------

project(iree-bare-metal-arm VERSION 1.0 LANGUAGES C ASM)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(BUILD_WITH_CMSIS "Builds the sample with CMSIS" OFF)
option(BUILD_WITH_LIBOPENCM3 "Builds the sample with libopencm3" OFF)
option(BUILD_BINARY "Build binary files in addition to ELF files" ON)
option(BUILD_IHEX "Build ihex files in addition to ELF files" ON)
option(USE_UART0 "Initializes UART0" OFF)
option(USE_UART1 "Initializes UART1" OFF)
option(USE_UART2 "Initializes UART2" OFF)

message(STATUS "Building a ${CMAKE_BUILD_TYPE} build")

if(BUILD_WITH_CMSIS AND BUILD_WITH_LIBOPENCM3)
  message(FATAL_ERROR "Only one of CMSIS or libopencm3 can be used at the same time")
endif()

if(NOT BUILD_WITH_CMSIS AND NOT BUILD_WITH_LIBOPENCM3)
  message(FATAL_ERROR "Either set BUILD_WITH_CMSIS or BUILD_WITH_LIBOPENCM3 to ON")
endif()

# Define ARM_TARGET property
set(ARM_TARGET STM32F407 CACHE STRING "Target for which the samples are build")
set(ARM_TARGET_VALUES "NRF52840;STM32F407;STM32F411XE;STM32F446;STM32F4XX;STM32F746;STM32L476;CORSTONE-300" CACHE INTERNAL "List of possible targets")
set_property(CACHE ARM_TARGET PROPERTY STRINGS ${ARM_TARGET_VALUES})
string(TOUPPER "${ARM_TARGET}" ARM_TARGET)

# Check if the target is supported
if(NOT ARM_TARGET IN_LIST ARM_TARGET_VALUES)
  message(FATAL_ERROR "${ARM_TARGET} is not supported!")
else()
  message(STATUS "Building samples for ${ARM_TARGET}")
endif()

if(NOT (ARM_TARGET MATCHES "^STM32F4") AND BUILD_WITH_LIBOPENCM3)
  message(FATAL_ERROR "Only STM32F4 series devices are supported via libopencm3")
endif()

# We don't support UART0 on any STM32 board
if(ARM_TARGET MATCHES "^STM32" AND USE_UART0)
  message(FATAL_ERROR "UART0 is not supported for STM32 boards")
endif()
# We don't support UART1 on STM32F4 Discovery kit
if(ARM_TARGET STREQUAL "STM32F407" AND USE_UART1)
  message(FATAL_ERROR "UART1 is not supported for STM32F407")
endif()
# We don't support UART1 and UART2 on NRF52840
if(ARM_TARGET STREQUAL "NRF52840" AND (USE_UART1 OR USE_UART2))
  message(FATAL_ERROR "UART1 and UART2 are not supported for NF528400")
endif()

# Define CLOCK_SOURCE property
set(CLOCK_SOURCE HSI CACHE STRING "Clock source to be configured")
set(CLOCK_SOURCE_VALUES "HSI;HSE;PLL_HSI;PLL_HSE" CACHE INTERNAL "List of possible clock sources")
set_property(CACHE CLOCK_SOURCE PROPERTY STRINGS ${CLOCK_SOURCE_VALUES})
string(TOUPPER "${CLOCK_SOURCE}" CLOCK_SOURCE)

# Check if the clock source is supported
if(NOT CLOCK_SOURCE IN_LIST CLOCK_SOURCE_VALUES)
  message(FATAL_ERROR "Clock source ${CLOCK_SOURCE} is not supported!")
else()
  message(STATUS "Configuring clock source as ${CLOCK_SOURCE}")
endif()

# We don't support clock sources HSE and PLL_HSE on STM32L476
if(ARM_TARGET STREQUAL "STM32L476" AND (CLOCK_SOURCE STREQUAL "HSE" OR CLOCK_SOURCE STREQUAL "PLL_HSE"))
  message(FATAL_ERROR "Clock sources other than HSI and PLL_HSI are not supported on STM32L476")
endif()

# Set executable suffix to elf
set(CMAKE_EXECUTABLE_SUFFIX ".elf")

#-------------------------------------------------------------------------------
# Utility definitions
#-------------------------------------------------------------------------------

list(APPEND CMAKE_MODULE_PATH
  ${CMAKE_CURRENT_LIST_DIR}/build_tools/cmake/
)

include(add_binary)
include(add_ihex)

#-------------------------------------------------------------------------------
# Third-party dependencies
#-------------------------------------------------------------------------------

# Extend module path to allow submodules to find AddMLIR
list(APPEND CMAKE_MODULE_PATH "${PROJECT_BINARY_DIR}/lib/cmake/mlir")

# IREE-specific settings
set(IREE_BUILD_COMPILER OFF)
set(IREE_ENABLE_MLIR OFF)
set(IREE_BUILD_SAMPLES OFF)
set(IREE_ENABLE_THREADING OFF)
set(IREE_BUILD_TESTS OFF)
set(IREE_BUILD_BINDINGS_TFLITE OFF)
set(IREE_HAL_DRIVER_DEFAULTS OFF)
set(IREE_HAL_EXECUTABLE_LOADER_DEFAULTS OFF)

# IREE-specific definitions
add_compile_definitions(IREE_PLATFORM_GENERIC)
add_compile_definitions(IREE_SYNCHRONIZATION_DISABLE_UNSAFE=1)
add_compile_definitions(IREE_STATUS_FEATURES=0)
add_compile_definitions(FLATCC_USE_GENERIC_ALIGNED_ALLOC)

if(BUILD_WITH_CMSIS)
  if(ARM_TARGET MATCHES "^STM32F4")
    add_subdirectory(build_tools/third_party/cmsis_device_f4 EXCLUDE_FROM_ALL)
  endif()
  if(ARM_TARGET MATCHES "^STM32F7")
    add_subdirectory(build_tools/third_party/cmsis_device_f7 EXCLUDE_FROM_ALL)
  endif()
  if(ARM_TARGET MATCHES "^STM32H7")
    add_subdirectory(build_tools/third_party/cmsis_device_h7 EXCLUDE_FROM_ALL)
  endif()
  if(ARM_TARGET MATCHES "^STM32L4")
    add_subdirectory(build_tools/third_party/cmsis_device_l4 EXCLUDE_FROM_ALL)
  endif()
  if(ARM_TARGET STREQUAL "CORSTONE-300")
    add_subdirectory(build_tools/third_party/ethos-u-core-platform EXCLUDE_FROM_ALL)
  endif()
  if(ARM_TARGET MATCHES "^NRF")
    add_subdirectory(build_tools/third_party/nrfx EXCLUDE_FROM_ALL)
  endif()
endif()

if(BUILD_WITH_LIBOPENCM3)
  add_subdirectory(build_tools/third_party/libopencm3 EXCLUDE_FROM_ALL)
endif()

add_subdirectory(third_party/iree EXCLUDE_FROM_ALL)

#-------------------------------------------------------------------------------
# Top-level components
#-------------------------------------------------------------------------------

add_subdirectory(samples)
add_subdirectory(utils)
