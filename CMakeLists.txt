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
option(BUILD_WITH_CRT0 "Builds the sample with crt0.s" OFF)
option(BUILD_WITH_LIBOPENCM3 "Builds the sample with libopencm3" OFF)
option(BUILD_IHEX "Build ihex files in addition to ELF files" ON)
option(USE_UART1 "Initializes UART1" OFF)
option(USE_UART2 "Initializes UART2" OFF)

if(
  (BUILD_WITH_CMSIS AND BUILD_WITH_CRT0) OR
  (BUILD_WITH_CMSIS AND BUILD_WITH_LIBOPENCM3) OR
  (BUILD_WITH_CRT0 AND BUILD_WITH_LIBOPENCM3)
)
  message(FATAL_ERROR "Only one of CMSIS, crt0.s or libopencm3 can be used at the same time")
endif()

if(NOT BUILD_WITH_CMSIS AND NOT BUILD_WITH_CRT0 AND NOT BUILD_WITH_LIBOPENCM3)
  message(FATAL_ERROR "Either set BUILD_WITH_CMSIS, BUILD_WITH_CRT0 or BUILD_WITH_LIBOPENCM3 to ON")
endif()

# Define ARM_TARGET property
set(ARM_TARGET STM32F407 CACHE STRING "Target for which the samples are build")
set(ARM_TARGET_VALUES "STM32F407;STM32F411XE;STM32F446;STM32F4XX;STM32F746" CACHE INTERNAL "List of possible targets")
set_property(CACHE ARM_TARGET PROPERTY STRINGS ${ARM_TARGET_VALUES})
string(TOUPPER "${ARM_TARGET}" ARM_TARGET)

# Check if the target is supported
if(NOT ARM_TARGET IN_LIST ARM_TARGET_VALUES)
  message(FATAL_ERROR "${ARM_TARGET} is not supported!")
else()
  message(STATUS "Building samples for ${ARM_TARGET}")
endif()

# Check if the combination of library and target is supported
if(NOT (ARM_TARGET STREQUAL "STM32F407" OR ARM_TARGET STREQUAL "STM32F4XX") AND BUILD_WITH_CRT0)
  message(FATAL_ERROR "Devices other than STM32F407 or STM32F4XX are not supported by crt0.s")
endif()

if(NOT (ARM_TARGET MATCHES "^STM32F4") AND BUILD_WITH_LIBOPENCM3)
  message(FATAL_ERROR "Only STM32F4 series devices are supported via libopencm3")
endif()

# We don't support UART1 on STM32F4 Discovery kit
if(ARM_TARGET STREQUAL "STM32F407" AND USE_UART1)
  message(FATAL_ERROR "UART1 is not supported for STM32F407")
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

#-------------------------------------------------------------------------------
# Utility definitions
#-------------------------------------------------------------------------------

list(APPEND CMAKE_MODULE_PATH
  ${CMAKE_CURRENT_LIST_DIR}/build_tools/cmake/
)

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

# IREE-specific definitions
add_definitions(-DIREE_PLATFORM_GENERIC)
add_definitions(-DIREE_SYNCHRONIZATION_DISABLE_UNSAFE=1)
add_definitions(-DIREE_STATUS_FEATURES=0)
add_definitions(-DFLATCC_USE_GENERIC_ALIGNED_ALLOC)

add_subdirectory(build_tools/third_party/cmsis_device_f4 EXCLUDE_FROM_ALL)
add_subdirectory(build_tools/third_party/cmsis_device_f7 EXCLUDE_FROM_ALL)
add_subdirectory(build_tools/third_party/custom_crt0 EXCLUDE_FROM_ALL)
add_subdirectory(build_tools/third_party/libopencm3 EXCLUDE_FROM_ALL)
add_subdirectory(third_party/iree EXCLUDE_FROM_ALL)

#-------------------------------------------------------------------------------
# Top-level components
#-------------------------------------------------------------------------------

add_subdirectory(samples)
add_subdirectory(utils)
