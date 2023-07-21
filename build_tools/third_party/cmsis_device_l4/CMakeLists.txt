# SPDX-FileCopyrightText: 2022 Fraunhofer-Gesellschaft zur Förderung der angewandten Forschung e.V.
#
# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(cmsis_device_l4_SOURCE_DIR "${iree-bare-metal-arm_SOURCE_DIR}/third_party/cmsis_device_l4/")
set(CMSIS_5_SOURCE_DIR "${iree-bare-metal-arm_SOURCE_DIR}/third_party/CMSIS_5/")

#-------------------------------------------------------------------------------
# STM32L4R5xx
#-------------------------------------------------------------------------------

add_library(cmsis_device_l4r5xx STATIC "")

target_sources(cmsis_device_l4r5xx
  PRIVATE
    ${cmsis_device_l4_SOURCE_DIR}/Source/Templates/system_stm32l4xx.c
    ${CMSIS_5_SOURCE_DIR}/Device/ARM/ARMCM4/Source/startup_ARMCM4.c
)

target_include_directories(cmsis_device_l4r5xx
  PUBLIC
    ${cmsis_device_l4_SOURCE_DIR}/Include/
    ${CMSIS_5_SOURCE_DIR}/CMSIS/Core/Include/
  PRIVATE
    ${CMSIS_5_SOURCE_DIR}/Device/ARM/ARMCM4/Include
)

target_compile_definitions(cmsis_device_l4r5xx
  PUBLIC
    STM32L4R5xx
  PRIVATE
    ARMCM4_FP
)

# Allow ld to find generic linker script
target_link_directories(cmsis_device_l4r5xx INTERFACE "${iree-bare-metal-arm_SOURCE_DIR}/build_tools/")

#-------------------------------------------------------------------------------
# STM32L476xx
#-------------------------------------------------------------------------------

add_library(cmsis_device_l476xx STATIC "")

target_sources(cmsis_device_l476xx
  PRIVATE
    ${cmsis_device_l4_SOURCE_DIR}/Source/Templates/system_stm32l4xx.c
    ${CMSIS_5_SOURCE_DIR}/Device/ARM/ARMCM4/Source/startup_ARMCM4.c
)

target_include_directories(cmsis_device_l476xx
  PUBLIC
    ${cmsis_device_l4_SOURCE_DIR}/Include/
    ${CMSIS_5_SOURCE_DIR}/CMSIS/Core/Include/
  PRIVATE
    ${CMSIS_5_SOURCE_DIR}/Device/ARM/ARMCM4/Include
)

target_compile_definitions(cmsis_device_l476xx
  PUBLIC
    STM32L476xx
  PRIVATE
    ARMCM4_FP
)

# Allow ld to find generic linker script
target_link_directories(cmsis_device_l476xx INTERFACE "${iree-bare-metal-arm_SOURCE_DIR}/build_tools/")
