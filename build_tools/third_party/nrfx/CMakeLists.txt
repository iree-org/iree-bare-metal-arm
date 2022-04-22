# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(nrfx_DIR "${PROJECT_SOURCE_DIR}/third_party/nrfx/")
set(CMSIS_5_SOURCE_DIR "${PROJECT_SOURCE_DIR}/third_party/CMSIS_5/")

#-------------------------------------------------------------------------------
# nRF52840
#-------------------------------------------------------------------------------

add_library(nrfx_nrf52840 STATIC "")

target_sources(nrfx_nrf52840
  PRIVATE
    ${nrfx_DIR}/mdk/system_nrf52840.c
    ${nrfx_DIR}/mdk/gcc_startup_nrf52840.S
)

target_include_directories(nrfx_nrf52840
  PUBLIC
    ${CMSIS_5_SOURCE_DIR}/CMSIS/Core/Include/
  PRIVATE
    ${nrfx_DIR}/mdk/
)

target_compile_definitions(nrfx_nrf52840
  PRIVATE
    NRF52840_XXAA
)
