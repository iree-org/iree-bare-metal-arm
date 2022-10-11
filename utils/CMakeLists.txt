# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

if(BUILD_WITH_CMSIS)
  if(ARM_TARGET STREQUAL "NRF52840")
    set(UTILS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/nrf52840_cmsis.c)
    set(UTILS_SRC "${UTILS_SRC};${CMAKE_CURRENT_SOURCE_DIR}/nrf_write_cmsis.c")
  elseif(ARM_TARGET MATCHES "^STM32F4")
    set(UTILS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/stm32f4_cmsis.c)
  elseif(ARM_TARGET MATCHES "^STM32F7")
    set(UTILS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/stm32f7_cmsis.c)
  elseif(ARM_TARGET MATCHES "^STM32L4")
    set(UTILS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/stm32l4_cmsis.c)
  elseif(ARM_TARGET STREQUAL "CORSTONE-300")
    set(UTILS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/corstone-300.c)
  endif()
  if(ARM_TARGET MATCHES "^STM32")
    set(UTILS_SRC "${UTILS_SRC};${CMAKE_CURRENT_SOURCE_DIR}/stm32_write_cmsis.c")
  endif()
elseif(BUILD_WITH_LIBOPENCM3)
  set(UTILS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/stm32f4_libopencm3.c)
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
