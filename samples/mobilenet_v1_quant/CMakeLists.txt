# Copyright 2023 Fraunhofer-Gesellschaft zur Förderung der angewandten
#                Forschung e.V.
#
# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#-------------------------------------------------------------------------------
# Generate model input
#-------------------------------------------------------------------------------

set(_CONVERT_INPUT_SCRIPT "${CMAKE_SOURCE_DIR}/build_tools/convert-image-to-c-array.py")
set(_MODEL_INPUT "${CMAKE_CURRENT_SOURCE_DIR}/dog.jpg")

add_custom_command(
  OUTPUT
    ${CMAKE_CURRENT_BINARY_DIR}/model_input.h
  COMMAND
    ${_CONVERT_INPUT_SCRIPT} --resize 224x224 --data-format uint8 ${_MODEL_INPUT} > ${CMAKE_CURRENT_BINARY_DIR}/model_input.h
  DEPENDS ${_CONVERT_INPUT_SCRIPT} ${_MODEL_INPUT}
)

add_library(mobilenet_v1_model_input
  INTERFACE
    ${CMAKE_CURRENT_BINARY_DIR}/model_input.h
)

#-------------------------------------------------------------------------------
# VMVX sample
#-------------------------------------------------------------------------------

if(IREE_HAL_EXECUTABLE_LOADER_VMVX_MODULE)

  vmvx_module(
    NAME
      mobilenet_v1_bytecode_module_vmvx_c
    SRC
      "${CMAKE_CURRENT_SOURCE_DIR}/mobilenet_v1_0.25_224_quantized_1_default_1.mlir"
    C_IDENTIFIER
      "mobilenet_v1_bytecode_module_vmvx"
    FLAGS
      "--iree-input-type=tosa"
  )

  add_executable(mobilenet_v1_bytecode_vmvx "")
  target_sources(mobilenet_v1_bytecode_vmvx
    PRIVATE
      mobilenet_v1.c
      mobilenet_v1_vmvx.c
      device_vmvx_sync.c
  )

  set_target_properties(mobilenet_v1_bytecode_vmvx PROPERTIES OUTPUT_NAME mobilenet_v1_bytecode_vmvx)

  target_include_directories(mobilenet_v1_bytecode_vmvx
    PRIVATE
      ${CMAKE_CURRENT_BINARY_DIR}
  )

  target_link_libraries(mobilenet_v1_bytecode_vmvx
    PRIVATE
      mobilenet_v1_bytecode_module_vmvx_c
      mobilenet_v1_model_input
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

  add_binary(mobilenet_v1_bytecode_vmvx)
  add_ihex(mobilenet_v1_bytecode_vmvx)

endif()


#-------------------------------------------------------------------------------
# Static library sample
#-------------------------------------------------------------------------------

add_executable(mobilenet_v1_bytecode_static "")

target_sources(mobilenet_v1_bytecode_static
  PRIVATE
    mobilenet_v1.c
    mobilenet_v1_bytecode.c
    device_static_sync.c
)

static_module(
  NAME
    mobilenet_v1_bytecode_module_static_c
  SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/mobilenet_v1_0.25_224_quantized_1_default_1.mlir"
  C_IDENTIFIER
    "mobilenet_v1"
  FLAGS
    "--iree-input-type=tosa"
)

target_include_directories(mobilenet_v1_bytecode_static
  PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(mobilenet_v1_bytecode_static
  PRIVATE
    mobilenet_v1_bytecode_module_static_c
    mobilenet_v1_model_input
    iree::runtime
    iree::hal::drivers::local_sync::sync_driver
    iree::hal::local::loaders::static_library_loader
    firmware
    utils
    write
)

add_binary(mobilenet_v1_bytecode_static)
add_ihex(mobilenet_v1_bytecode_static)

#-------------------------------------------------------------------------------
# Static library sample, EmitC
#-------------------------------------------------------------------------------

add_executable(mobilenet_v1_bytecode_static_c "")

target_sources(mobilenet_v1_bytecode_static_c
  PRIVATE
    mobilenet_v1.c
    mobilenet_v1_c_module.c
    device_static_sync.c
)

static_module(
  NAME
    mobilenet_v1_bytecode_module_static_c_module
  SRC
  "${CMAKE_CURRENT_SOURCE_DIR}/mobilenet_v1_0.25_224_quantized_1_default_1.mlir"
  C_IDENTIFIER
    "mobilenet_v1"
  FLAGS
    "--iree-input-type=tosa"
  EMITC
)

target_include_directories(mobilenet_v1_bytecode_static_c
  PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(mobilenet_v1_bytecode_static_c
  PRIVATE
    mobilenet_v1_bytecode_module_static_c_module
    mobilenet_v1_model_input
    iree::runtime
    iree::hal::drivers::local_sync::sync_driver
    iree::hal::local::loaders::static_library_loader
    firmware
    utils
    write
)

add_binary(mobilenet_v1_bytecode_static_c)
add_ihex(mobilenet_v1_bytecode_static_c)
