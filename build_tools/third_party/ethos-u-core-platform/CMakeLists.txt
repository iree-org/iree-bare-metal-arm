# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(ethos_u_core_platform_SOURCE_DIR "${PROJECT_SOURCE_DIR}/third_party/ethos-u-core-platform/")
set(CMSIS_5_SOURCE_DIR "${PROJECT_SOURCE_DIR}/third_party/CMSIS_5/")

if(${ARM_CPU} STREQUAL "cortex-m55")
  set(_ARM_CPU "ARMCM55")
  set(_ARM_FEATURES "")
elseif(${ARM_CPU} STREQUAL "cortex-m4")
  set(_ARM_CPU "ARMCM4")
  set(_ARM_FEATURES "_FP")
elseif(ARM_TARGET STREQUAL "CORSTONE-300")
  # _ARM_CPU has only be set properly if building for Corstone
  message(FATAL_ERROR "'${ARM_CPU}' is not supported.")
endif()

#-------------------------------------------------------------------------------
# ethos_u_core_platform_cmsis
#-------------------------------------------------------------------------------

add_library(ethos_u_core_platform_cmsis INTERFACE)

target_include_directories(ethos_u_core_platform_cmsis INTERFACE
  ${CMSIS_5_SOURCE_DIR}/CMSIS/Core/Include
  ${CMSIS_5_SOURCE_DIR}/Device/ARM/${_ARM_CPU}/Include
)

target_sources(ethos_u_core_platform_cmsis INTERFACE
  ${CMSIS_5_SOURCE_DIR}/Device/ARM/${_ARM_CPU}/Source/startup_${_ARM_CPU}.c
  ${CMSIS_5_SOURCE_DIR}/Device/ARM/${_ARM_CPU}/Source/system_${_ARM_CPU}.c
)

target_compile_definitions(ethos_u_core_platform_cmsis INTERFACE ${_ARM_CPU}${_ARM_FEATURES})

#-------------------------------------------------------------------------------
# ethosu_uart_cmsdk_apb
#-------------------------------------------------------------------------------

add_subdirectory(${ethos_u_core_platform_SOURCE_DIR}/drivers/uart ${CMAKE_CURRENT_BINARY_DIR}/drivers/uart EXCLUDE_FROM_ALL)

#-------------------------------------------------------------------------------
# ethos_u_core_platform_corstone-300
#-------------------------------------------------------------------------------

add_library(ethos_u_core_platform_corstone-300 STATIC "")

target_sources(ethos_u_core_platform_corstone-300 PRIVATE
  ${ethos_u_core_platform_SOURCE_DIR}/targets/corstone-300/retarget.c
)

target_link_libraries(ethos_u_core_platform_corstone-300
  PUBLIC
    ethosu_uart_common
  PRIVATE
    ethos_u_core_platform_cmsis
    ethosu_uart_cmsdk_apb
)
