# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

if(BUILD_WITH_CMSIS)
  set(UTILS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/utils/utils_cmsis.c)
  set(CONDITIONAL_DEP cmsis_device_f4)
endif()

if(BUILD_WITH_CRT0)
  set(UTILS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/utils/utils_crt0.c)
  set(CONDITIONAL_DEP custom_crt0)
endif()

if(BUILD_WITH_LIBOPENCM3)
  set(UTILS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/utils/utils_libopencm3.c)
  set(CONDITIONAL_DEP stm32f4)
endif()

if(EXISTS ${IREE_HOST_BINARY_ROOT}/bin/iree-translate)
  # Use `iree-translate` if installed to IREE_HOST_BINARY_ROOT.
  set(_TRANSLATE_TOOL_EXECUTABLE ${IREE_HOST_BINARY_ROOT}/bin/iree-translate)
else()
  # Use `iree-translate` provided via a snapshot.
  find_program(_TRANSLATE_TOOL_EXECUTABLE iree-translate)
  if(_TRANSLATE_TOOL_EXECUTABLE STREQUAL _TRANSLATE_TOOL_EXECUTABLE-NOTFOUND)
    message(FATAL_ERROR "Could not find iree-translate.")
  endif()
endif()

add_subdirectory(simple_embedding)
add_subdirectory(static_library)
