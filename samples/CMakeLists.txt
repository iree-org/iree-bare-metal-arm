# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

add_executable(sample_vmvx_sync "")
add_executable(sample_embedded_sync "")
add_executable(sample_static_library "")

if(BUILD_WITH_CMSIS)
  target_compile_definitions(sample_vmvx_sync PRIVATE -DBUILD_WITH_CMSIS)
  target_compile_definitions(sample_embedded_sync PRIVATE -DBUILD_WITH_CMSIS)
  target_compile_definitions(sample_static_library PRIVATE -DBUILD_WITH_CMSIS)
  set(CONDITIONAL_DEP cmsis_device_f4)
endif()

if(BUILD_WITH_LIBOPENCM3)
  target_compile_definitions(sample_vmvx_sync PRIVATE -DBUILD_WITH_LIBOPENCM3)
  target_compile_definitions(sample_embedded_sync PRIVATE -DBUILD_WITH_LIBOPENCM3)
  target_compile_definitions(sample_static_library PRIVATE -DBUILD_WITH_LIBOPENCM3)
  set(CONDITIONAL_DEP stm32f4)
endif()

set(_TRANSLATE_TOOL_EXECUTABLE ${IREE_HOST_BINARY_ROOT}/bin/iree-translate)

#-------------------------------------------------------------------------------
# VMVX sample
#-------------------------------------------------------------------------------

target_sources(sample_vmvx_sync
  PRIVATE
    simple_embedding.c
    ${IREE_SOURCE_DIR}/iree/samples/simple_embedding/device_vmvx_sync.c
)

set_target_properties(sample_vmvx_sync PROPERTIES OUTPUT_NAME sample_vmvx_sync)

target_link_libraries(sample_vmvx_sync
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

#-------------------------------------------------------------------------------
# DYLIB sample
#-------------------------------------------------------------------------------

target_sources(sample_embedded_sync
  PRIVATE
    simple_embedding.c
    device_embedded_sync.c
)

set(_TRANSLATE_ARGS)
list(APPEND _TRANSLATE_ARGS "-iree-input-type=mhlo")
list(APPEND _TRANSLATE_ARGS "-iree-mlir-to-vm-bytecode-module")
list(APPEND _TRANSLATE_ARGS "-iree-hal-target-backends=dylib-llvm-aot")
list(APPEND _TRANSLATE_ARGS "-iree-llvm-target-triple=armv7em-pc-linux-elf")
list(APPEND _TRANSLATE_ARGS "-iree-llvm-target-float-abi=hard")
list(APPEND _TRANSLATE_ARGS "-iree-llvm-debug-symbols=false")
list(APPEND _TRANSLATE_ARGS "-iree-vm-bytecode-module-strip-source-map=true")
list(APPEND _TRANSLATE_ARGS "-iree-vm-emit-polyglot-zip=false")
list(APPEND _TRANSLATE_ARGS "${IREE_SOURCE_DIR}/iree/samples/simple_embedding/simple_embedding_test.mlir")
list(APPEND _TRANSLATE_ARGS "-o")
list(APPEND _TRANSLATE_ARGS "simple_embedding_test_module_dylib_arm_32.vmfb")

add_custom_command(
  OUTPUT "simple_embedding_test_module_dylib_arm_32.vmfb"
  COMMAND ${_TRANSLATE_TOOL_EXECUTABLE} ${_TRANSLATE_ARGS}
)

set(_GEN_EMBED_ARG)
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
  simple_embedding_test_bytecode_module_dylib
  iree::base
  iree::hal
  iree::hal::local
  iree::hal::local::loaders::embedded_library_loader
  iree::hal::local::sync_driver
  iree::modules::hal
  iree::vm
  iree::vm::bytecode_module
  ${CONDITIONAL_DEP}
)

#-------------------------------------------------------------------------------
# Static library sample
#-------------------------------------------------------------------------------

target_sources(sample_static_library
  PRIVATE
    static_library_demo.c
)

set(_TRANSLATE_ARGS)
list(APPEND _TRANSLATE_ARGS "-iree-mlir-to-vm-bytecode-module")
list(APPEND _TRANSLATE_ARGS "-iree-hal-target-backends=dylib-llvm-aot")
list(APPEND _TRANSLATE_ARGS "-iree-llvm-target-triple=armv7em-pc-linux-elf")
list(APPEND _TRANSLATE_ARGS "-iree-llvm-target-float-abi=hard")
list(APPEND _TRANSLATE_ARGS "-iree-llvm-link-embedded=false")
list(APPEND _TRANSLATE_ARGS "-iree-llvm-link-static")
# TODO(marbre): Remove after IREE got cleaned up
list(APPEND _TRANSLATE_ARGS "-iree-llvm-system-linker-path=${ARM_TOOLCHAIN_ROOT}/bin/arm-none-eabi-ld")
list(APPEND _TRANSLATE_ARGS "-iree-llvm-static-library-output-path=simple_mul.o")
list(APPEND _TRANSLATE_ARGS "${IREE_SOURCE_DIR}/iree/samples/static_library/simple_mul.mlir")
list(APPEND _TRANSLATE_ARGS "-o")
list(APPEND _TRANSLATE_ARGS "simple_mul.vmfb")

add_custom_command(
  OUTPUT
    ${CMAKE_CURRENT_BINARY_DIR}/simple_mul.h
    ${CMAKE_CURRENT_BINARY_DIR}/simple_mul.o
    ${CMAKE_CURRENT_BINARY_DIR}/simple_mul.vmfb
  COMMAND ${_TRANSLATE_TOOL_EXECUTABLE} ${_TRANSLATE_ARGS}
)

add_library(simple_mul
  STATIC
  ${CMAKE_CURRENT_BINARY_DIR}/simple_mul.o)

SET_TARGET_PROPERTIES(
  simple_mul
  PROPERTIES
  LINKER_LANGUAGE C)

set(_GEN_EMBED_ARGS)
list(APPEND _GEN_EMBED_ARGS "--output_header=simple_mul_c.h")
list(APPEND _GEN_EMBED_ARGS "--output_impl=simple_mul_c.c")
list(APPEND _GEN_EMBED_ARGS "--identifier=iree_samples_static_library_simple_mul")
list(APPEND _GEN_EMBED_ARGS "--flatten")
list(APPEND _GEN_EMBED_ARGS "simple_mul.vmfb")

add_custom_command(
  OUTPUT
    "simple_mul_c.h"
    "simple_mul_c.c"
  COMMAND generate_embed_data ${_GEN_EMBED_ARGS}
  DEPENDS generate_embed_data simple_mul.vmfb
)

add_library(simple_mul_c STATIC "")
target_sources(simple_mul_c
  PRIVATE
    simple_mul_c.c
    simple_mul_c.h
)

target_include_directories(sample_static_library
  PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(sample_static_library
  simple_mul_c
  iree::runtime
  iree::hal::local::loaders::static_library_loader
  iree::hal::local::sync_driver
  simple_mul
  ${CONDITIONAL_DEP}
)
