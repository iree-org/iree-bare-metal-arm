# Copyright 2023 Fraunhofer-Gesellschaft zur Förderung der angewandten
#                Forschung e.V.
#
# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#-------------------------------------------------------------------------------
# Model input
#-------------------------------------------------------------------------------

set(_CONVERT_INPUT_SCRIPT "${CMAKE_SOURCE_DIR}/build_tools/convert-image-to-c-array.py")
set(_MODEL_PATH "${CMAKE_SOURCE_DIR}/models/mobilenet_v1_quant/mobilenet_v1_0.25_224_quantized_1_default_1.mlir")
set(_MODEL_INPUT "${CMAKE_SOURCE_DIR}/models/mobilenet_v1_quant/dog.jpg")
add_custom_command(
  OUTPUT
    ${CMAKE_CURRENT_BINARY_DIR}/model_input.h
  COMMAND
    ${_CONVERT_INPUT_SCRIPT} --resize 224x224 --data-format uint8 ${_MODEL_INPUT} > ${CMAKE_CURRENT_BINARY_DIR}/model_input.h
  DEPENDS ${_CONVERT_INPUT_SCRIPT} ${_MODEL_INPUT}
)

add_library(mobilenet_v1_quant_model_input
  INTERFACE
    ${CMAKE_CURRENT_BINARY_DIR}/model_input.h
)

#-------------------------------------------------------------------------------
# MobileNet V1 sample
#-------------------------------------------------------------------------------

add_executable(mobilenet_v1_quant_static_library "")

target_sources(mobilenet_v1_quant_static_library
  PRIVATE
    static_library_mobilenet_v1_quant.c
    create_bytecode_module.c
)

set(_COMPILE_ARGS)
list(APPEND _COMPILE_ARGS "--iree-input-type=tosa")
list(APPEND _COMPILE_ARGS "--output-format=vm-bytecode")
list(APPEND _COMPILE_ARGS "--iree-hal-target-backends=llvm-cpu")
list(APPEND _COMPILE_ARGS "--iree-llvm-target-triple=${IREE_LLVM_TARGET_TRIPLE}")
list(APPEND _COMPILE_ARGS "--iree-llvm-target-cpu=${ARM_CPU}")
list(APPEND _COMPILE_ARGS "--iree-llvm-target-float-abi=${IREE_LLVM_TARGET_FLOAT_ABI}")
list(APPEND _COMPILE_ARGS "--iree-llvm-link-embedded=false")
list(APPEND _COMPILE_ARGS "--iree-llvm-link-static")
list(APPEND _COMPILE_ARGS "--iree-llvm-static-library-output-path=mobilenet_v1_quant.o")
list(APPEND _COMPILE_ARGS "${_MODEL_PATH}")
list(APPEND _COMPILE_ARGS "-o")
list(APPEND _COMPILE_ARGS "mobilenet_v1_quant.vmfb")

add_custom_command(
  OUTPUT
    ${CMAKE_CURRENT_BINARY_DIR}/mobilenet_v1_quant.h
    ${CMAKE_CURRENT_BINARY_DIR}/mobilenet_v1_quant.o
    ${CMAKE_CURRENT_BINARY_DIR}/mobilenet_v1_quant.vmfb
  COMMAND ${_COMPILE_TOOL_EXECUTABLE} ${_COMPILE_ARGS}
)

add_library(mobilenet_v1_quant
  STATIC
    ${CMAKE_CURRENT_BINARY_DIR}/mobilenet_v1_quant.o
)

set_target_properties(mobilenet_v1_quant
  PROPERTIES
    LINKER_LANGUAGE C
)

set(_GEN_EMBED_ARGS)
list(APPEND _GEN_EMBED_ARGS "--output_header=mobilenet_v1_quant_c.h")
list(APPEND _GEN_EMBED_ARGS "--output_impl=mobilenet_v1_quant_c.c")
list(APPEND _GEN_EMBED_ARGS "--identifier=iree_samples_static_library_mobilenet_v1_quant")
list(APPEND _GEN_EMBED_ARGS "--flatten")
list(APPEND _GEN_EMBED_ARGS "mobilenet_v1_quant.vmfb")

add_custom_command(
  OUTPUT
    "mobilenet_v1_quant_c.h"
    "mobilenet_v1_quant_c.c"
  COMMAND generate_embed_data ${_GEN_EMBED_ARGS}
  DEPENDS generate_embed_data mobilenet_v1_quant.vmfb
)

add_library(mobilenet_v1_quant_c STATIC "")
target_sources(mobilenet_v1_quant_c
  PRIVATE
    mobilenet_v1_quant_c.c
    mobilenet_v1_quant_c.h
)

target_include_directories(mobilenet_v1_quant_static_library
  PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(mobilenet_v1_quant_static_library
  PRIVATE
    mobilenet_v1_quant_c
    mobilenet_v1_quant_model_input
    iree::runtime
    iree::hal::drivers::local_sync::sync_driver
    iree::hal::local::loaders::static_library_loader
    mobilenet_v1_quant
    firmware
    utils
    write
)

add_binary(mobilenet_v1_quant_static_library)
add_ihex(mobilenet_v1_quant_static_library)

#-------------------------------------------------------------------------------
# Vision inference sample, EmitC
#-------------------------------------------------------------------------------

add_executable(mobilenet_v1_quant_static_library_c "")

set(_COMPILE_ARGS)
list(APPEND _COMPILE_ARGS "--iree-input-type=tosa")
list(APPEND _COMPILE_ARGS "--output-format=vm-c")
list(APPEND _COMPILE_ARGS "--iree-hal-target-backends=llvm-cpu")
list(APPEND _COMPILE_ARGS "--iree-llvm-target-triple=${IREE_LLVM_TARGET_TRIPLE}")
list(APPEND _COMPILE_ARGS "--iree-llvm-target-cpu=${ARM_CPU}")
list(APPEND _COMPILE_ARGS "--iree-llvm-target-float-abi=${IREE_LLVM_TARGET_FLOAT_ABI}")
list(APPEND _COMPILE_ARGS "--iree-llvm-link-embedded=false")
list(APPEND _COMPILE_ARGS "--iree-llvm-link-static")
list(APPEND _COMPILE_ARGS "--iree-llvm-static-library-output-path=mobilenet_v1_quant_c_module.o")
list(APPEND _COMPILE_ARGS "--iree-vm-target-index-bits=32")
list(APPEND _COMPILE_ARGS "${_MODEL_PATH}")
list(APPEND _COMPILE_ARGS "-o")
list(APPEND _COMPILE_ARGS "mobilenet_v1_quant_emitc.h")

add_custom_command(
  OUTPUT
    ${CMAKE_CURRENT_BINARY_DIR}/mobilenet_v1_quant_c_module.h
    ${CMAKE_CURRENT_BINARY_DIR}/mobilenet_v1_quant_c_module.o
    ${CMAKE_CURRENT_BINARY_DIR}/mobilenet_v1_quant_emitc.h
  COMMAND ${_COMPILE_TOOL_EXECUTABLE} ${_COMPILE_ARGS}
)

add_library(mobilenet_v1_quant_c_module
  STATIC
    ${CMAKE_CURRENT_BINARY_DIR}/mobilenet_v1_quant_c_module.o)

target_sources(mobilenet_v1_quant_c_module
  PRIVATE
    mobilenet_v1_quant_c_module.h
    mobilenet_v1_quant_emitc.h
)

target_compile_definitions(mobilenet_v1_quant_c_module
  PUBLIC
    EMITC_IMPLEMENTATION
)

set_target_properties(mobilenet_v1_quant_c_module
  PROPERTIES
    LINKER_LANGUAGE C
)

target_sources(mobilenet_v1_quant_static_library_c
  PRIVATE
    static_library_mobilenet_v1_quant.c
    create_c_module.c
)

target_include_directories(mobilenet_v1_quant_static_library_c
  PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(mobilenet_v1_quant_static_library_c
  PRIVATE
    mobilenet_v1_quant_c_module
    mobilenet_v1_quant_model_input
    iree::runtime
    iree::hal::drivers::local_sync::sync_driver
    iree::hal::local::loaders::static_library_loader
    iree::vm::shims_emitc
    firmware
    utils
    write
)

add_binary(mobilenet_v1_quant_static_library_c)
add_ihex(mobilenet_v1_quant_static_library_c)
