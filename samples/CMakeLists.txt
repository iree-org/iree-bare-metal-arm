# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

add_library(firmware INTERFACE "")

if(BUILD_WITH_CMSIS)
  if(ARM_TARGET STREQUAL "STM32F407" OR ARM_TARGET STREQUAL "STM32F4XX")
    set(CONDITIONAL_DEP cmsis_device_f407xx)
  elseif(ARM_TARGET STREQUAL "STM32F411XE")
    set(CONDITIONAL_DEP cmsis_device_f411xe)
  elseif(ARM_TARGET STREQUAL "STM32F446")
    set(CONDITIONAL_DEP cmsis_device_f446xx)
  elseif(ARM_TARGET STREQUAL "STM32F746")
    set(CONDITIONAL_DEP cmsis_device_f746xx)
  elseif(ARM_TARGET STREQUAL "STM32L476")
    set(CONDITIONAL_DEP cmsis_device_l476xx)
  elseif(ARM_TARGET STREQUAL "CORSTONE-300")
    set(CONDITIONAL_DEP ethos_u_core_platform_corstone-300)
  endif()
elseif(BUILD_WITH_LIBOPENCM3)
  set(CONDITIONAL_DEP stm32f4)
endif()

target_link_libraries(firmware
  INTERFACE
    ${CONDITIONAL_DEP}
)

if(EXISTS ${IREE_HOST_BINARY_ROOT}/bin/iree-compile)
  # Use `iree-compile` if installed to IREE_HOST_BINARY_ROOT.
  set(_COMPILE_TOOL_EXECUTABLE ${IREE_HOST_BINARY_ROOT}/bin/iree-compile)
else()
  # Use `iree-compile` provided via a snapshot.
  find_program(_COMPILE_TOOL_EXECUTABLE iree-compile)
  if(_COMPILE_TOOL_EXECUTABLE STREQUAL _COMPILE_TOOL_EXECUTABLE-NOTFOUND)
    message(FATAL_ERROR "Could not find iree-compile.")
  endif()
endif()

add_subdirectory(simple_embedding)
add_subdirectory(static_library)
add_subdirectory(vision_inference)
