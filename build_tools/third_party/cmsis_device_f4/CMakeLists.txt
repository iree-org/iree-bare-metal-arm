# SPDX-FileCopyrightText: 2021 Fraunhofer-Gesellschaft zur Förderung der angewandten Forschung e.V.
#
# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(cmsis_device_f4_SOURCE_DIR "${iree-bare-metal-arm_SOURCE_DIR}/third_party/cmsis_device_f4/")
set(CMSIS_5_SOURCE_DIR "${iree-bare-metal-arm_SOURCE_DIR}/third_party/CMSIS_5/")

#-------------------------------------------------------------------------------
# STM32F407xx
#-------------------------------------------------------------------------------

add_library(cmsis_device_f407xx STATIC "")

target_sources(cmsis_device_f407xx
  PRIVATE
    ${cmsis_device_f4_SOURCE_DIR}/Source/Templates/system_stm32f4xx.c
    ${CMSIS_5_SOURCE_DIR}/Device/ARM/ARMCM4/Source/startup_ARMCM4.c
)

target_include_directories(cmsis_device_f407xx
  PUBLIC
    ${cmsis_device_f4_SOURCE_DIR}/Include/
    ${CMSIS_5_SOURCE_DIR}/CMSIS/Core/Include/
  PRIVATE
    ${CMSIS_5_SOURCE_DIR}/Device/ARM/ARMCM4/Include
)

target_compile_definitions(cmsis_device_f407xx
  PUBLIC
    STM32F407xx
  PRIVATE
    HSI_VALUE=16000000
    HSE_VALUE=8000000
    ARMCM4_FP
)

# Allow ld to find generic linker script
target_link_directories(cmsis_device_f407xx INTERFACE "${iree-bare-metal-arm_SOURCE_DIR}/build_tools/")

#-------------------------------------------------------------------------------
# STM32F411xe
#-------------------------------------------------------------------------------

add_library(cmsis_device_f411xe STATIC "")

target_sources(cmsis_device_f411xe
  PRIVATE
    ${cmsis_device_f4_SOURCE_DIR}/Source/Templates/system_stm32f4xx.c
    ${CMSIS_5_SOURCE_DIR}/Device/ARM/ARMCM4/Source/startup_ARMCM4.c
)

target_include_directories(cmsis_device_f411xe
  PUBLIC
    ${cmsis_device_f4_SOURCE_DIR}/Include/
    ${CMSIS_5_SOURCE_DIR}/CMSIS/Core/Include/
  PRIVATE
    ${CMSIS_5_SOURCE_DIR}/Device/ARM/ARMCM4/Include
)

target_compile_definitions(cmsis_device_f411xe
  PUBLIC
    STM32F411xE
  PRIVATE
    HSI_VALUE=16000000
    HSE_VALUE=8000000
    ARMCM4_FP
)

# Allow ld to find generic linker script
target_link_directories(cmsis_device_f411xe INTERFACE "${iree-bare-metal-arm_SOURCE_DIR}/build_tools/")

#-------------------------------------------------------------------------------
# STM32F446xx
#-------------------------------------------------------------------------------

add_library(cmsis_device_f446xx STATIC "")

target_sources(cmsis_device_f446xx
  PRIVATE
    ${cmsis_device_f4_SOURCE_DIR}/Source/Templates/system_stm32f4xx.c
    ${CMSIS_5_SOURCE_DIR}/Device/ARM/ARMCM4/Source/startup_ARMCM4.c
)

target_include_directories(cmsis_device_f446xx
  PUBLIC
    ${cmsis_device_f4_SOURCE_DIR}/Include/
    ${CMSIS_5_SOURCE_DIR}/CMSIS/Core/Include/
  PRIVATE
    ${CMSIS_5_SOURCE_DIR}/Device/ARM/ARMCM4/Include
)

target_compile_definitions(cmsis_device_f446xx
  PUBLIC
    STM32F446xx
  PRIVATE
    HSI_VALUE=16000000
    HSE_VALUE=8000000
    ARMCM4_FP
)

# Allow ld to find generic linker script
target_link_directories(cmsis_device_f446xx INTERFACE "${iree-bare-metal-arm_SOURCE_DIR}/build_tools/")
