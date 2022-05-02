# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(libopencm3_SOURCE_DIR "${iree-bare-metal-arm_SOURCE_DIR}/third_party/libopencm3/")

#-------------------------------------------------------------------------------
# STM32F4
#-------------------------------------------------------------------------------

add_custom_target(libopencm3_stm32f4 make TARGETS=stm32/f4
  WORKING_DIRECTORY ${libopencm3_SOURCE_DIR}
  BYPRODUCTS ${libopencm3_SOURCE_DIR}/lib/libopencm3_stm32f4.a
)

add_library(stm32f4 STATIC IMPORTED GLOBAL)
add_dependencies(stm32f4 libopencm3_stm32f4)
set_property(TARGET stm32f4
  PROPERTY IMPORTED_LOCATION ${libopencm3_SOURCE_DIR}/lib/libopencm3_stm32f4.a)

target_compile_definitions(stm32f4 INTERFACE -DSTM32F4)
target_include_directories(stm32f4 INTERFACE ${libopencm3_SOURCE_DIR}/include)
target_link_directories(stm32f4 INTERFACE ${libopencm3_SOURCE_DIR}/lib)

#-------------------------------------------------------------------------------
# STM32F7
#-------------------------------------------------------------------------------

add_custom_target(libopencm3_stm32f7 make TARGETS=stm32/f7
  WORKING_DIRECTORY ${libopencm3_SOURCE_DIR}
  BYPRODUCTS ${libopencm3_SOURCE_DIR}/lib/libopencm3_stm32f7.a
)

add_library(stm32f7 STATIC IMPORTED GLOBAL)
add_dependencies(stm32f7 libopencm3_stm32f7)
set_property(TARGET stm32f7
  PROPERTY IMPORTED_LOCATION ${libopencm3_SOURCE_DIR}/lib/libopencm3_stm32f7.a)

target_compile_definitions(stm32f7 INTERFACE -DSTM32F7)
target_include_directories(stm32f7 INTERFACE ${libopencm3_SOURCE_DIR}/include)
target_link_directories(stm32f7 INTERFACE ${libopencm3_SOURCE_DIR}/lib)
