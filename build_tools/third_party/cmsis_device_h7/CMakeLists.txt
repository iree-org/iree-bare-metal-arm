# SPDX-FileCopyrightText: 2022 The IREE bare-metal Arm Authors
#
# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.h7a3xx
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(cmsis_device_h7_SOURCE_DIR "${iree-bare-metal-arm_SOURCE_DIR}/third_party/cmsis_device_h7/")
set(CMSIS_5_SOURCE_DIR "${iree-bare-metal-arm_SOURCE_DIR}/third_party/CMSIS_5/")

#-------------------------------------------------------------------------------
# STM32H7A3xx
#-------------------------------------------------------------------------------

add_library(cmsis_device_h7a3xx STATIC "")

target_sources(cmsis_device_h7a3xx
  PRIVATE
    ${cmsis_device_h7_SOURCE_DIR}/Source/Templates/system_stm32h7xx.c
    ${CMSIS_5_SOURCE_DIR}/Device/ARM/ARMCM7/Source/startup_ARMCM7.c
)

target_include_directories(cmsis_device_h7a3xx
  PUBLIC
    ${cmsis_device_h7_SOURCE_DIR}/Include/
    ${CMSIS_5_SOURCE_DIR}/CMSIS/Core/Include/
  PRIVATE
    ${CMSIS_5_SOURCE_DIR}/Device/ARM/ARMCM7/Include
)

target_compile_definitions(cmsis_device_h7a3xx
  PUBLIC
    STM32H7A3xx
  PRIVATE
    ARMCM7_DP
)

# Allow ld to find generic linker script
target_link_directories(cmsis_device_h7a3xx INTERFACE "${iree-bare-metal-arm_SOURCE_DIR}/build_tools/")

#-------------------------------------------------------------------------------
# STM32H7A3xxQ
#-------------------------------------------------------------------------------

add_library(cmsis_device_h7a3xxq STATIC "")

target_sources(cmsis_device_h7a3xxq
  PRIVATE
    ${cmsis_device_h7_SOURCE_DIR}/Source/Templates/system_stm32h7xx.c
    ${CMSIS_5_SOURCE_DIR}/Device/ARM/ARMCM7/Source/startup_ARMCM7.c
)

target_include_directories(cmsis_device_h7a3xxq
  PUBLIC
    ${cmsis_device_h7_SOURCE_DIR}/Include/
    ${CMSIS_5_SOURCE_DIR}/CMSIS/Core/Include/
  PRIVATE
    ${CMSIS_5_SOURCE_DIR}/Device/ARM/ARMCM7/Include
)

target_compile_definitions(cmsis_device_h7a3xxq
  PUBLIC
    STM32H7A3xxQ
  PRIVATE
    ARMCM7_DP
)

# Allow ld to find generic linker script
target_link_directories(cmsis_device_h7a3xxq INTERFACE "${iree-bare-metal-arm_SOURCE_DIR}/build_tools/")
