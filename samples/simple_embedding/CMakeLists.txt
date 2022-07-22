
# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#-------------------------------------------------------------------------------
# VMVX sample, float
#-------------------------------------------------------------------------------

if(IREE_HAL_EXECUTABLE_LOADER_VMVX_MODULE)

  add_executable(sample_vmvx_sync "")

  target_sources(sample_vmvx_sync
    PRIVATE
      simple_embedding_float.c
      device_vmvx_sync.c
  )

  set(_COMPILE_ARGS)
  list(APPEND _COMPILE_ARGS "--iree-input-type=mhlo")
  list(APPEND _COMPILE_ARGS "--iree-mlir-to-vm-bytecode-module")
  list(APPEND _COMPILE_ARGS "--iree-hal-target-backends=vmvx")
  list(APPEND _COMPILE_ARGS "${IREE_SOURCE_DIR}/samples/simple_embedding/simple_embedding_test.mlir")
  list(APPEND _COMPILE_ARGS "-o")
  list(APPEND _COMPILE_ARGS "simple_embedding_test_bytecode_module_vmvx.vmfb")

  add_custom_command(
    OUTPUT "simple_embedding_test_bytecode_module_vmvx.vmfb"
    COMMAND ${_COMPILE_TOOL_EXECUTABLE} ${_COMPILE_ARGS}
  )

  set(_GEN_EMBED_ARGS)
  list(APPEND _GEN_EMBED_ARGS "--output_header=simple_embedding_test_bytecode_module_vmvx_c.h")
  list(APPEND _GEN_EMBED_ARGS "--output_impl=simple_embedding_test_bytecode_module_vmvx_c.c")
  list(APPEND _GEN_EMBED_ARGS "--identifier=iree_samples_simple_embedding_test_module_vmvx")
  list(APPEND _GEN_EMBED_ARGS "--flatten")
  list(APPEND _GEN_EMBED_ARGS "simple_embedding_test_bytecode_module_vmvx.vmfb")

  add_custom_command(
    OUTPUT
      "simple_embedding_test_bytecode_module_vmvx_c.h"
      "simple_embedding_test_bytecode_module_vmvx_c.c"
    COMMAND generate_embed_data ${_GEN_EMBED_ARGS}
    DEPENDS generate_embed_data simple_embedding_test_bytecode_module_vmvx.vmfb
  )

  add_library(simple_embedding_test_bytecode_module_vmvx STATIC "")
  target_sources(simple_embedding_test_bytecode_module_vmvx
    PRIVATE
      simple_embedding_test_bytecode_module_vmvx_c.c
      simple_embedding_test_bytecode_module_vmvx_c.h
  )

  set_target_properties(sample_vmvx_sync PROPERTIES OUTPUT_NAME sample_vmvx_sync)

  target_include_directories(sample_vmvx_sync
    PRIVATE
      ${CMAKE_CURRENT_BINARY_DIR}
  )

  target_link_libraries(sample_vmvx_sync
    PRIVATE
      simple_embedding_test_bytecode_module_vmvx
      iree::base
      iree::hal
      iree::hal::drivers::local_sync::sync_driver
      iree::hal::local
      iree::hal::local::loaders::vmvx_module_loader
      iree::modules::hal
      iree::vm
      iree::vm::bytecode_module
      firmware
      utils
  )

  add_binary(sample_vmvx_sync)
  add_ihex(sample_vmvx_sync)

endif()

#-------------------------------------------------------------------------------
# DYLIB sample, int32
#-------------------------------------------------------------------------------

add_executable(sample_embedded_sync "")

target_sources(sample_embedded_sync
  PRIVATE
    simple_embedding_int.c
    device_embedded_sync.c
)

set(_COMPILE_ARGS)
list(APPEND _COMPILE_ARGS "--iree-input-type=mhlo")
list(APPEND _COMPILE_ARGS "--iree-mlir-to-vm-bytecode-module")
list(APPEND _COMPILE_ARGS "--iree-hal-target-backends=llvm-cpu")
list(APPEND _COMPILE_ARGS "--iree-llvm-target-triple=${IREE_LLVM_TARGET_TRIPLE}")
list(APPEND _COMPILE_ARGS "--iree-llvm-target-cpu=${ARM_CPU}")
list(APPEND _COMPILE_ARGS "--iree-llvm-target-float-abi=${IREE_LLVM_TARGET_FLOAT_ABI}")
list(APPEND _COMPILE_ARGS "--iree-llvm-debug-symbols=false")
list(APPEND _COMPILE_ARGS "--iree-vm-bytecode-module-strip-source-map=true")
list(APPEND _COMPILE_ARGS "--iree-vm-emit-polyglot-zip=false")
list(APPEND _COMPILE_ARGS "${CMAKE_CURRENT_SOURCE_DIR}/simple_embedding_int_test.mlir")
list(APPEND _COMPILE_ARGS "-o")
list(APPEND _COMPILE_ARGS "simple_embedding_test_module_dylib_arm_32.vmfb")

add_custom_command(
  OUTPUT "simple_embedding_test_module_dylib_arm_32.vmfb"
  COMMAND ${_COMPILE_TOOL_EXECUTABLE} ${_COMPILE_ARGS}
)

set(_GEN_EMBED_ARGS)
list(APPEND _GEN_EMBED_ARGS "--output_header=simple_embedding_test_module_dylib_arm_32.h")
list(APPEND _GEN_EMBED_ARGS "--output_impl=simple_embedding_test_module_dylib_arm_32.c")
list(APPEND _GEN_EMBED_ARGS "--identifier=iree_samples_simple_embedding_test_module_dylib_arm_32")
list(APPEND _GEN_EMBED_ARGS "--flatten")
list(APPEND _GEN_EMBED_ARGS "simple_embedding_test_module_dylib_arm_32.vmfb")

add_custom_command(
  OUTPUT
    "simple_embedding_test_module_dylib_arm_32.h"
    "simple_embedding_test_module_dylib_arm_32.c"
  COMMAND generate_embed_data ${_GEN_EMBED_ARGS}
  DEPENDS generate_embed_data simple_embedding_test_module_dylib_arm_32.vmfb
)

add_library(simple_embedding_test_bytecode_module_dylib STATIC "")
target_sources(simple_embedding_test_bytecode_module_dylib
  PRIVATE
    simple_embedding_test_module_dylib_arm_32.c
    simple_embedding_test_module_dylib_arm_32.h
)

target_include_directories(sample_embedded_sync
  PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(sample_embedded_sync
  PRIVATE
    simple_embedding_test_bytecode_module_dylib
    iree::base
    iree::hal
    iree::hal::drivers::local_sync::sync_driver
    iree::hal::local
    iree::hal::local::loaders::embedded_elf_loader
    iree::modules::hal
    iree::vm
    iree::vm::bytecode_module
    firmware
    utils
)

add_binary(sample_embedded_sync)
add_ihex(sample_embedded_sync)
