# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

if(BUILD_WITH_CMSIS)
  if(ARM_TARGET MATCHES "^STM32F4")
    set(UTILS_SRC ${CMAKE_SOURCE_DIR}/utils/stm32f4_cmsis.c)
  elseif(ARM_TARGET MATCHES "^STM32F7")
    set(UTILS_SRC ${CMAKE_SOURCE_DIR}/utils/stm32f7_cmsis.c)
  endif()
elseif(BUILD_WITH_CRT0)
  set(UTILS_SRC ${CMAKE_SOURCE_DIR}/utils/stm32f4_crt0.c)
elseif(BUILD_WITH_LIBOPENCM3)
  set(UTILS_SRC ${CMAKE_SOURCE_DIR}/utils/stm32f4_libopencm3.c)
endif()

add_library(utils INTERFACE "")
target_sources(utils INTERFACE
  ${UTILS_SRC}
)

if(USE_UART1)
  target_compile_definitions(utils INTERFACE USE_UART1)
endif()

if(USE_UART2)
  target_compile_definitions(utils INTERFACE USE_UART2)
endif()

if(CLOCK_SOURCE STREQUAL "HSI")
  target_compile_definitions(utils INTERFACE USE_HSI_CLOCK)
elseif(CLOCK_SOURCE STREQUAL "HSE")
  target_compile_definitions(utils INTERFACE USE_HSE_CLOCK)
elseif(CLOCK_SOURCE STREQUAL "PLL_HSI")
  target_compile_definitions(utils INTERFACE USE_PLL_HSI_CLOCK)
elseif(CLOCK_SOURCE STREQUAL "PLL_HSE")
  target_compile_definitions(utils INTERFACE USE_PLL_HSE_CLOCK)
endif()
