# Copyright 2021 Fraunhofer-Gesellschaft zur Förderung der angewandten Forschung e.V.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required (VERSION 3.13.4)

# https://android.googlesource.com/platform/ndk/+/d45abffea8df0a52d8623c3aa28fd5f950a94ea7%5E%21/
#
# CMake invokes the toolchain file twice during the first build, but only once
# during subsequent rebuilds. This was causing the various flags to be added
# twice on the first build, and on a rebuild ninja would see only one set of the
# flags and rebuild the world.
# https://github.com/android-ndk/ndk/issues/323
if(ARM_NONE_EABI_GCC_TOOLCHAIN_INCLUDED)
	return()
endif(ARM_NONE_EABI_GCC_TOOLCHAIN_INCLUDED)
set(ARM_NONE_EABI_GCC_TOOLCHAIN_INCLUDED TRUE)

set(CMAKE_CROSSCOMPILING TRUE)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm CACHE STRING "" FORCE)

set(ARM_TOOLCHAIN_NAME gcc-arm-none-eabi)

set(ARM_TOOLCHAIN_ROOT "" CACHE PATH "ARM compiler path")
set(CMAKE_FIND_ROOT_PATH ${ARM_TOOLCHAIN_ROOT})
list(APPEND CMAKE_PREFIX_PATH "${ARM_TOOLCHAIN_ROOT}")

# Search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# For libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_EXE_LINKER_FLAGS "-specs=nosys.specs" CACHE INTERNAL "")

# Define ARM_CPU property
set(ARM_CPU cortex-m4 CACHE STRING "CPU for which to compile")
set(ARM_CPU_VALUES "cortex-m4;cortex-m7;cortex-m55" CACHE INTERNAL "List of possible CPUs")
set_property(CACHE ARM_CPU PROPERTY STRINGS ${ARM_CPU_VALUES})
string(TOLOWER "${ARM_CPU}" ARM_CPU)

# Check if the CPU is supported
if(NOT ARM_CPU IN_LIST ARM_CPU_VALUES)
  message(FATAL_ERROR "${ARM_CPU} is not supported!")
else()
  message(STATUS "Building for ${ARM_CPU}")
endif()

set(CMAKE_C_COMPILER "${ARM_TOOLCHAIN_ROOT}/bin/arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "${ARM_TOOLCHAIN_ROOT}/bin/arm-none-eabi-g++")
set(CMAKE_AR "${ARM_TOOLCHAIN_ROOT}/bin/arm-none-eabi-ar")
set(CMAKE_RANLIB "${ARM_TOOLCHAIN_ROOT}/bin/arm-none-eabi-ranlib")
set(CMAKE_STRIP "${ARM_TOOLCHAIN_ROOT}/bin/arm-none-eabi-strip")
set(CMAKE_SYSTEM_LIBRARY_PATH "${ARM_TOOLCHAIN_ROOT}/arm-none-eabi/usr/lib")

set(ARM_COMPILER_FLAGS)
set(ARM_COMPILER_FLAGS_CXX)
set(ARM_COMPILER_FLAGS_DEBUG)
set(ARM_COMPILER_FLAGS_RELEASE)
set(ARM_LINKER_FLAGS "-lc -lm ${CUSTOM_ARM_LINKER_FLAGS} -T ${LINKER_SCRIPT}")
set(ARM_LINKER_FLAGS_EXE)

if(ARM_CPU STREQUAL "cortex-m4")
  set(ARM_COMPILER_FLAGS "${ARM_COMPILER_FLAGS} -mthumb -march=armv7e-m -mfloat-abi=hard -mfpu=fpv4-sp-d16")
elseif(ARM_CPU STREQUAL "cortex-m7")
  set(ARM_COMPILER_FLAGS "${ARM_COMPILER_FLAGS} -mthumb -march=armv7e-m -mfloat-abi=hard")
  # The FPU is selected based on the ARM_TARGET.
  if(ARM_TARGET STREQUAL "STM32F746")
    # Single-precision FPU
    set(ARM_COMPILER_FLAGS "${ARM_COMPILER_FLAGS} -mfpu=fpv5-sp-d16")
  else()
    # Single- and double-precision FPU
    set(ARM_COMPILER_FLAGS "${ARM_COMPILER_FLAGS} -mfpu=fpv5-d16")
  endif()
elseif(ARM_CPU STREQUAL "cortex-m55")
  set(ARM_COMPILER_FLAGS "${ARM_COMPILER_FLAGS} -mthumb -mcpu=cortex-m55 -mfloat-abi=hard")
endif()

set(ARM_COMPILER_FLAGS "${ARM_COMPILER_FLAGS} -DIREE_TIME_NOW_FN=\"\{ return 0; \}\" -DIREE_WAIT_UNTIL_FN=wait_until")
set(ARM_COMPILER_FLAGS "${ARM_COMPILER_FLAGS} -Wl,--gc-sections -ffunction-sections -fdata-sections -mno-unaligned-access")

if(ARM_CPU STREQUAL "cortex-m55")
  set(IREE_LLVM_TARGET_TRIPLE "armv8.1m.main-pc-linux-elf")
else()
  set(IREE_LLVM_TARGET_TRIPLE "armv7em-pc-linux-elf")
endif()

set(CMAKE_C_FLAGS             "${ARM_COMPILER_FLAGS} ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS           "${ARM_COMPILER_FLAGS} ${ARM_COMPILER_FLAGS_CXX} ${CMAKE_CXX_FLAGS}")
set(CMAKE_ASM_FLAGS           "${ARM_COMPILER_FLAGS} ${CMAKE_ASM_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG       "${ARM_COMPILER_FLAGS_DEBUG} ${CMAKE_C_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_DEBUG     "${ARM_COMPILER_FLAGS_DEBUG} ${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_ASM_FLAGS_DEBUG     "${ARM_COMPILER_FLAGS_DEBUG} ${CMAKE_ASM_FLAGS_DEBUG}")
set(CMAKE_C_FLAGS_RELEASE     "${ARM_COMPILER_FLAGS_RELEASE} ${CMAKE_C_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_RELEASE   "${ARM_COMPILER_FLAGS_RELEASE} ${CMAKE_CXX_FLAGS_RELEASE}")
set(CMAKE_ASM_FLAGS_RELEASE   "${ARM_COMPILER_FLAGS_RELEASE} ${CMAKE_ASM_FLAGS_RELEASE}")
set(CMAKE_SHARED_LINKER_FLAGS "${ARM_LINKER_FLAGS} ${CMAKE_SHARED_LINKER_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS "${ARM_LINKER_FLAGS} ${CMAKE_MODULE_LINKER_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS    "${ARM_LINKER_FLAGS} ${ARM_LINKER_FLAGS_EXE} ${CMAKE_EXE_LINKER_FLAGS}")
