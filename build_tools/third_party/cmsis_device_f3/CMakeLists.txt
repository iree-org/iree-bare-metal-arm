# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(cmsis_device_f3_SOURCE_DIR "${PROJECT_SOURCE_DIR}/third_party/cmsis_device_f3/")
set(CMSIS_5_SOURCE_DIR "${PROJECT_SOURCE_DIR}/third_party/CMSIS_5/")

#-------------------------------------------------------------------------------
# STM32F303xE
#-------------------------------------------------------------------------------

add_library(cmsis_device_f303xe STATIC "")

target_sources(cmsis_device_f303xe PRIVATE
  ${cmsis_device_f3_SOURCE_DIR}/Source/Templates/system_stm32f3xx.c
  ${cmsis_device_f3_SOURCE_DIR}/Source/Templates/gcc/startup_stm32f303xe.s
)

target_include_directories(cmsis_device_f303xe
  PUBLIC
  ${cmsis_device_f3_SOURCE_DIR}/Include/
  ${CMSIS_5_SOURCE_DIR}/CMSIS/Core/Include/
)

target_compile_definitions(cmsis_device_f303xe PUBLIC -DSTM32F303xE)

# Allow ld to find generic linker script
target_link_directories(cmsis_device_f303xe INTERFACE "${PROJECT_SOURCE_DIR}/build_tools/")
