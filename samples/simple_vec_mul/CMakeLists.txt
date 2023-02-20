# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#-------------------------------------------------------------------------------
# VMVX sample
#-------------------------------------------------------------------------------

if(IREE_HAL_EXECUTABLE_LOADER_VMVX_MODULE)

  vmvx_module(
    NAME
      simple_mul_int_bytecode_module_vmvx_c
    SRC
      "${CMAKE_CURRENT_SOURCE_DIR}/simple_mul_int.mlir"
    C_IDENTIFIER
      "simple_mul_int_bytecode_module_vmvx"
    FLAGS
      "--iree-input-type=mhlo"
  )

  add_executable(simple_vec_mul_int_bytecode_vmvx "")
  target_sources(simple_vec_mul_int_bytecode_vmvx
    PRIVATE
      simple_mul_int.c
      simple_mul_int_vmvx.c
      device_vmvx_sync.c
  )

  set_target_properties(simple_vec_mul_int_bytecode_vmvx PROPERTIES OUTPUT_NAME simple_vec_mul_int_bytecode_vmvx)

  target_include_directories(simple_vec_mul_int_bytecode_vmvx
    PRIVATE
      ${CMAKE_CURRENT_BINARY_DIR}
  )

  target_link_libraries(simple_vec_mul_int_bytecode_vmvx
    PRIVATE
      simple_mul_int_bytecode_module_vmvx_c
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
      write
  )

  add_binary(simple_vec_mul_int_bytecode_vmvx)
  add_ihex(simple_vec_mul_int_bytecode_vmvx)

endif()


#-------------------------------------------------------------------------------
# Static library sample
#-------------------------------------------------------------------------------

add_executable(simple_vec_mul_int_bytecode_static "")

target_sources(simple_vec_mul_int_bytecode_static
  PRIVATE
    simple_mul_int.c
    simple_mul_int_bytecode.c
    device_static_sync.c
)

static_module(
  NAME
    simple_mul_int_bytecode_module_static_c
  SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/simple_mul_int.mlir"
  C_IDENTIFIER
    "simple_mul_int"
  FLAGS
    "--iree-input-type=mhlo"
)

target_include_directories(simple_vec_mul_int_bytecode_static
  PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(simple_vec_mul_int_bytecode_static
  PRIVATE
    simple_mul_int_bytecode_module_static_c
    iree::runtime
    iree::hal::drivers::local_sync::sync_driver
    iree::hal::local::loaders::static_library_loader
    firmware
    utils
    write
)

add_binary(simple_vec_mul_int_bytecode_static)
add_ihex(simple_vec_mul_int_bytecode_static)

#-------------------------------------------------------------------------------
# Static library sample, EmitC
#-------------------------------------------------------------------------------

add_executable(simple_vec_mul_int_bytecode_static_c "")

target_sources(simple_vec_mul_int_bytecode_static_c
  PRIVATE
    simple_mul_int.c
    simple_mul_int_c_module.c
    device_static_sync.c
)

static_module(
  NAME
    simple_mul_int_bytecode_module_static_c_module
  SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/simple_mul_int.mlir"
  C_IDENTIFIER
    "simple_mul_int"
  FLAGS
    "--iree-input-type=mhlo"
  EMITC
)

target_include_directories(simple_vec_mul_int_bytecode_static_c
  PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(simple_vec_mul_int_bytecode_static_c
  PRIVATE
    simple_mul_int_bytecode_module_static_c_module
    iree::runtime
    iree::hal::drivers::local_sync::sync_driver
    iree::hal::local::loaders::static_library_loader
    firmware
    utils
    write
)

add_binary(simple_vec_mul_int_bytecode_static_c)
add_ihex(simple_vec_mul_int_bytecode_static_c)
