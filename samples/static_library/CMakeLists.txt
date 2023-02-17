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

static_module(
  NAME
    simple_mul_module
  SRC
    "${IREE_SOURCE_DIR}/samples/static_library/simple_mul.mlir"
  C_IDENTIFIER
    "iree_samples_static_library_simple_mul"
)

target_include_directories(sample_static_library
  PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(sample_static_library
  PRIVATE
    simple_mul_module
    iree::runtime
    iree::hal::drivers::local_sync::sync_driver
    iree::hal::local::loaders::static_library_loader
    firmware
    utils
    write
)

add_binary(sample_static_library)
add_ihex(sample_static_library)

#-------------------------------------------------------------------------------
# Static library sample, EmitC
#-------------------------------------------------------------------------------

add_executable(sample_static_library_c "")

static_module(
  NAME
    simple_mul_c_module
  SRC
    "${IREE_SOURCE_DIR}/samples/static_library/simple_mul.mlir"
  EMITC
)

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
    iree::hal::drivers::local_sync::sync_driver
    iree::hal::local::loaders::static_library_loader
    iree::vm::shims_emitc
    firmware
    utils
    write
)

add_binary(sample_static_library_c)
add_ihex(sample_static_library_c)
