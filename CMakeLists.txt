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

# Extend module path to allow submodules to find AddMLIR
list(APPEND CMAKE_MODULE_PATH "${PROJECT_BINARY_DIR}/lib/cmake/mlir")

# IREE-specific settings
set(IREE_BUILD_COMPILER OFF)
set(IREE_ENABLE_MLIR OFF)
set(IREE_BUILD_SAMPLES ON)
set(IREE_BUILD_EMBEDDING_SAMPLES ON)
set(IREE_ENABLE_THREADING OFF)
set(IREE_BUILD_TESTS OFF)
set(IREE_BUILD_BINDINGS_TFLITE OFF)

# IREE-specific definitions
add_definitions(-DIREE_PLATFORM_GENERIC)
add_definitions(-DIREE_SYNCHRONIZATION_DISABLE_UNSAFE=1)
add_definitions(-DIREE_STATUS_FEATURES=0)
add_definitions(-DFLATCC_USE_GENERIC_ALIGNED_ALLOC)

#-------------------------------------------------------------------------------
# Third-party dependencies
#-------------------------------------------------------------------------------

add_subdirectory(build_tools/third_party/cmsis_device_f4 EXCLUDE_FROM_ALL)
add_subdirectory(build_tools/third_party/custom_crt0 EXCLUDE_FROM_ALL)
add_subdirectory(build_tools/third_party/libopencm3 EXCLUDE_FROM_ALL)
add_subdirectory(third_party/iree EXCLUDE_FROM_ALL)

#-------------------------------------------------------------------------------
# Top-level components
#-------------------------------------------------------------------------------

add_subdirectory(samples)
