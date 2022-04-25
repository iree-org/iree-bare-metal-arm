# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#-------------------------------------------------------------------------------
# Static library sample
#-------------------------------------------------------------------------------

add_executable(sample_static_library "")

target_sources(sample_static_library
  PRIVATE
    static_library_demo.c
    create_bytecode_module.c
)

set(_TRANSLATE_ARGS)
list(APPEND _TRANSLATE_ARGS "-iree-mlir-to-vm-bytecode-module")
list(APPEND _TRANSLATE_ARGS "-iree-hal-target-backends=dylib-llvm-aot")
list(APPEND _TRANSLATE_ARGS "-iree-llvm-target-triple=armv7em-pc-linux-elf")
list(APPEND _TRANSLATE_ARGS "-iree-llvm-target-float-abi=hard")
list(APPEND _TRANSLATE_ARGS "-iree-llvm-link-embedded=false")
list(APPEND _TRANSLATE_ARGS "-iree-llvm-link-static")
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

set_target_properties(
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
  PRIVATE
    simple_mul_c
    iree::runtime
    iree::hal::local::loaders::static_library_loader
    iree::hal::local::sync_driver
    simple_mul
    ${CONDITIONAL_DEP}
    utils
)

add_binary(sample_static_library)
add_ihex(sample_static_library)

#-------------------------------------------------------------------------------
# Static library sample, EmitC
#-------------------------------------------------------------------------------

add_executable(sample_static_library_c "")

set(_TRANSLATE_ARGS)
list(APPEND _TRANSLATE_ARGS "-iree-mlir-to-vm-c-module")
list(APPEND _TRANSLATE_ARGS "-iree-hal-target-backends=dylib-llvm-aot")
list(APPEND _TRANSLATE_ARGS "-iree-llvm-target-triple=armv7em-pc-linux-elf")
list(APPEND _TRANSLATE_ARGS "-iree-llvm-target-float-abi=hard")
list(APPEND _TRANSLATE_ARGS "-iree-llvm-link-embedded=false")
list(APPEND _TRANSLATE_ARGS "-iree-llvm-link-static")
list(APPEND _TRANSLATE_ARGS "-iree-llvm-static-library-output-path=simple_mul_c_module.o")
list(APPEND _TRANSLATE_ARGS "${IREE_SOURCE_DIR}/iree/samples/static_library/simple_mul.mlir")
list(APPEND _TRANSLATE_ARGS "-o")
list(APPEND _TRANSLATE_ARGS "simple_mul_emitc.h")

add_custom_command(
  OUTPUT
    ${CMAKE_CURRENT_BINARY_DIR}/simple_mul_c_module.h
    ${CMAKE_CURRENT_BINARY_DIR}/simple_mul_c_module.o
    ${CMAKE_CURRENT_BINARY_DIR}/simple_mul_emitc.h
  COMMAND ${_TRANSLATE_TOOL_EXECUTABLE} ${_TRANSLATE_ARGS}
)

add_library(simple_mul_c_module
  STATIC
  ${CMAKE_CURRENT_BINARY_DIR}/simple_mul_c_module.o)

target_sources(simple_mul_c_module
  PRIVATE
    simple_mul_c_module.h
    simple_mul_emitc.h
)

target_compile_definitions(simple_mul_c_module
  PUBLIC
    EMITC_IMPLEMENTATION
)

set_target_properties(
  simple_mul_c_module
  PROPERTIES
  LINKER_LANGUAGE C)

target_sources(sample_static_library_c
  PRIVATE
    static_library_demo.c
    create_c_module.c
)

target_include_directories(sample_static_library_c
  PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(sample_static_library_c
  PRIVATE
    simple_mul_c_module
    iree::runtime
    iree::hal::local::loaders::static_library_loader
    iree::hal::local::sync_driver
    iree::vm::shims_emitc
    ${CONDITIONAL_DEP}
    utils
)

add_binary(sample_static_library_c)
add_ihex(sample_static_library_c)
