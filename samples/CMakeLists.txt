# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

add_executable(simple_embedding "")

if(BUILD_WITH_CMSIS)
  target_compile_definitions(simple_embedding PRIVATE -DBUILD_WITH_CMSIS)
  set(CONDITIONAL_DEP cmsis_device_f4)
endif()

if(BUILD_WITH_LIBOPENCM3)
  target_compile_definitions(simple_embedding PRIVATE -DBUILD_WITH_LIBOPENCM3)
  set(CONDITIONAL_DEP stm32f4)
endif()

target_sources(simple_embedding
  PRIVATE
    simple_embedding.c
    ${iree_SOURCE_DIR}/iree/samples/simple_embedding/device_vmvx_sync.c
)

set_target_properties(simple_embedding PROPERTIES OUTPUT_NAME simple_embedding)

target_link_libraries(simple_embedding
  iree::samples::simple_embedding::simple_embedding_test_bytecode_module_vmvx_c
  iree::base
  iree::hal
  iree::hal::local
  iree::hal::local::loaders::vmvx_module_loader
  iree::hal::local::sync_driver
  iree::modules::hal
  iree::vm
  iree::vm::bytecode_module
  ${CONDITIONAL_DEP}
)
