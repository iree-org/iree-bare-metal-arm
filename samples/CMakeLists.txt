# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

if(BUILD_WITH_CMSIS)
  if(ARM_TARGET MATCHES "^STM32F3")
    set(UTILS_SRC ${CMAKE_SOURCE_DIR}/utils/stm32f3_cmsis.c)
  elseif(ARM_TARGET MATCHES "^STM32F4")
    set(UTILS_SRC ${CMAKE_SOURCE_DIR}/utils/stm32f4_cmsis.c)
  elseif(ARM_TARGET MATCHES "^STM32F7")
    set(UTILS_SRC ${CMAKE_SOURCE_DIR}/utils/stm32f7_cmsis.c)
  endif()
  if(ARM_TARGET STREQUAL "STM32F303XE")
    set(CONDITIONAL_DEP cmsis_device_f303xe)
  elseif(ARM_TARGET STREQUAL "STM32F407" OR ARM_TARGET STREQUAL "STM32F4XX")
    set(CONDITIONAL_DEP cmsis_device_f407xx)
  elseif(ARM_TARGET STREQUAL "STM32F411XE")
    set(CONDITIONAL_DEP cmsis_device_f411xe)
  elseif(ARM_TARGET STREQUAL "STM32F446")
    set(CONDITIONAL_DEP cmsis_device_f446xx)
  elseif(ARM_TARGET STREQUAL "STM32F746")
    set(CONDITIONAL_DEP cmsis_device_f746xx)
  endif()
endif()

if(BUILD_WITH_CRT0)
  set(UTILS_SRC ${CMAKE_SOURCE_DIR}/utils/stm32f4_crt0.c)
  set(CONDITIONAL_DEP custom_crt0)
endif()

if(BUILD_WITH_LIBOPENCM3)
  set(UTILS_SRC ${CMAKE_SOURCE_DIR}/utils/stm32f4_libopencm3.c)
  set(CONDITIONAL_DEP stm32f4)
endif()

if(EXISTS ${IREE_HOST_BINARY_ROOT}/bin/iree-compile)
  # Use `iree-compile` if installed to IREE_HOST_BINARY_ROOT.
  set(_TRANSLATE_TOOL_EXECUTABLE ${IREE_HOST_BINARY_ROOT}/bin/iree-compile)
else()
  # Use `iree-compile` provided via a snapshot.
  find_program(_TRANSLATE_TOOL_EXECUTABLE iree-compile)
  if(_TRANSLATE_TOOL_EXECUTABLE STREQUAL _TRANSLATE_TOOL_EXECUTABLE-NOTFOUND)
    message(FATAL_ERROR "Could not find iree-compile.")
  endif()
endif()

add_subdirectory(simple_embedding)
add_subdirectory(static_library)
