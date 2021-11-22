# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(cmsis_device_f4_SOURCE_DIR "${PROJECT_SOURCE_DIR}/third_party/cmsis_device_f4/")
set(CMSIS_5_SOURCE_DIR "${PROJECT_SOURCE_DIR}/third_party/CMSIS_5/")

add_library(cmsis_device_f4 STATIC "")

target_sources(cmsis_device_f4 PRIVATE
  ${cmsis_device_f4_SOURCE_DIR}/Source/Templates/system_stm32f4xx.c
  ${cmsis_device_f4_SOURCE_DIR}/Source/Templates/gcc/startup_stm32f407xx.s
)

target_include_directories(cmsis_device_f4
  PUBLIC
  ${cmsis_device_f4_SOURCE_DIR}/Include/
  ${CMSIS_5_SOURCE_DIR}/CMSIS/Core/Include/
)

target_compile_definitions(cmsis_device_f4 PUBLIC -DSTM32F407xx)

# Allow ld to find generic linker script
target_link_directories(cmsis_device_f4 INTERFACE "${PROJECT_SOURCE_DIR}/build_tools/")
