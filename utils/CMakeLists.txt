# SPDX-FileCopyrightText: 2022 Fraunhofer-Gesellschaft zur Förderung der angewandten Forschung e.V.
#
# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

if(BUILD_WITH_CMSIS)
  if(ARM_TARGET STREQUAL "NRF52840")
    set(UTILS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/nrf52840_cmsis.c)
    set(WRITE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/write.c)
  elseif(ARM_TARGET MATCHES "^STM32F4")
    set(UTILS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/stm32f4_cmsis.c)
  elseif(ARM_TARGET MATCHES "^STM32F7")
    set(UTILS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/stm32f7_cmsis.c)
  elseif(ARM_TARGET MATCHES "^STM32L4")
    set(UTILS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/stm32l4_cmsis.c)
  elseif(ARM_TARGET STREQUAL "CORSTONE-300")
    set(UTILS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/corstone-300.c)
    # `_write` is provided by `ethos_u_core_platform_corstone-300`
    set(WRITE_SRC "${CMAKE_CURRENT_BINARY_DIR}/write_dummy.c")
    file(WRITE ${WRITE_SRC} "")
  endif()
  if(ARM_TARGET MATCHES "^STM32")
    set(WRITE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/write_usart.c)
  endif()
elseif(BUILD_WITH_LIBOPENCM3)
  set(UTILS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/stm32f4_libopencm3.c)
  # `_write` is implemented in `tm32f4_libopencm3.c`
  set(WRITE_SRC "${CMAKE_CURRENT_BINARY_DIR}/write_dummy.c")
  file(WRITE ${WRITE_SRC} "")
endif()

add_library(utils STATIC "")
target_sources(utils
  PRIVATE
    ${UTILS_SRC}
)

target_link_libraries(utils
  PRIVATE
    firmware
)

if(ARM_TARGET STREQUAL "NRF52840")
  target_link_libraries(utils PRIVATE nrfx_uart)
endif()

if(USE_UART0)
  target_compile_definitions(utils PRIVATE USE_UART0)
endif()

if(USE_UART1)
  target_compile_definitions(utils PRIVATE USE_UART1)
endif()

if(USE_UART2)
  target_compile_definitions(utils PRIVATE USE_UART2)
endif()

if(CLOCK_SOURCE STREQUAL "HSI")
  target_compile_definitions(utils PRIVATE USE_HSI_CLOCK)
elseif(CLOCK_SOURCE STREQUAL "HSE")
  target_compile_definitions(utils PRIVATE USE_HSE_CLOCK)
elseif(CLOCK_SOURCE STREQUAL "PLL_HSI")
  target_compile_definitions(utils PRIVATE USE_PLL_HSI_CLOCK)
elseif(CLOCK_SOURCE STREQUAL "PLL_HSE")
  target_compile_definitions(utils PRIVATE USE_PLL_HSE_CLOCK)
endif()

add_library(write STATIC "")
target_sources(write
  PRIVATE
    ${WRITE_SRC}
)
