# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(cmsis_device_f7_SOURCE_DIR "${PROJECT_SOURCE_DIR}/third_party/cmsis_device_f7/")
set(CMSIS_5_SOURCE_DIR "${PROJECT_SOURCE_DIR}/third_party/CMSIS_5/")

#-------------------------------------------------------------------------------
# STM32F746xx
#-------------------------------------------------------------------------------

add_library(cmsis_device_f746xx STATIC "")

target_sources(cmsis_device_f746xx
  PRIVATE
    ${cmsis_device_f7_SOURCE_DIR}/Source/Templates/system_stm32f7xx.c
    ${CMSIS_5_SOURCE_DIR}/Device/ARM/ARMCM7/Source/startup_ARMCM7.c
)

target_include_directories(cmsis_device_f746xx
  PUBLIC
    ${cmsis_device_f7_SOURCE_DIR}/Include/
    ${CMSIS_5_SOURCE_DIR}/CMSIS/Core/Include/
  PRIVATE
    ${CMSIS_5_SOURCE_DIR}/Device/ARM/ARMCM7/Include
)

target_compile_definitions(cmsis_device_f746xx
  PUBLIC
    STM32F746xx
  PRIVATE
    ARMCM7_SP
)

# Allow ld to find generic linker script
target_link_directories(cmsis_device_f746xx INTERFACE "${PROJECT_SOURCE_DIR}/build_tools/")
