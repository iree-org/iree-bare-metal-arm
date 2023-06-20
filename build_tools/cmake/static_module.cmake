# Copyright 2023 Fraunhofer-Gesellschaft zur Förderung der angewandten Forschung e.V.
#
# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

# vmvx_module()
#
# CMake function to create a static module.

function(static_module)
  cmake_parse_arguments(
    _RULE
    "EMITC;INLINE_HAL"
    "NAME;SRC;C_IDENTIFIER"
    "FLAGS"
    ${ARGN}
  )

  set(_COMPILE_ARGS ${_RULE_FLAGS})
  set(_O_FILE_NAME "${_RULE_NAME}.o")
  set(_H_FILE_NAME "${_RULE_NAME}.h")

  if(_RULE_EMITC)
    list(APPEND _COMPILE_ARGS "--output-format=vm-c")
    list(APPEND _COMPILE_ARGS "--iree-vm-target-index-bits=32")

    set(_MODULE_NAME "${_RULE_NAME}_emitc.h")
  else()
    list(APPEND _COMPILE_ARGS "--output-format=vm-bytecode")

    set(_MODULE_NAME "${_RULE_NAME}.vmfb")
    set(_GEN_H_FILE_NAME "${_RULE_NAME}_c.h")
    set(_GEN_C_FILE_NAME "${_RULE_NAME}_c.c")
  endif()
  
  list(APPEND _COMPILE_ARGS "--iree-hal-target-backends=llvm-cpu")
  list(APPEND _COMPILE_ARGS "--iree-llvmcpu-target-triple=${IREE_LLVM_TARGET_TRIPLE}")
  list(APPEND _COMPILE_ARGS "--iree-llvmcpu-target-cpu=${ARM_CPU}")
  list(APPEND _COMPILE_ARGS "--iree-llvmcpu-target-float-abi=${IREE_LLVM_TARGET_FLOAT_ABI}")
  list(APPEND _COMPILE_ARGS "--iree-llvmcpu-link-embedded=false")
  list(APPEND _COMPILE_ARGS "--iree-llvmcpu-link-static")
  if(_RULE_INLINE_HAL)
    list(APPEND _COMPILE_ARGS "--iree-execution-model=inline-dynamic")
  endif()
  list(APPEND _COMPILE_ARGS "--iree-llvmcpu-static-library-output-path=${_O_FILE_NAME}")
  list(APPEND _COMPILE_ARGS "${_RULE_SRC}")
  list(APPEND _COMPILE_ARGS "-o")
  list(APPEND _COMPILE_ARGS "${_MODULE_NAME}")

  add_custom_command(
    OUTPUT
      ${_MODULE_NAME}
      ${_O_FILE_NAME}
      ${_H_FILE_NAME}
    COMMAND ${_COMPILE_TOOL_EXECUTABLE} ${_COMPILE_ARGS}
  )

  add_library(${_RULE_NAME}
    STATIC
      ${_O_FILE_NAME}
  )

  set_target_properties(${_RULE_NAME}
    PROPERTIES
      LINKER_LANGUAGE C
  )

  set(_SRC_FILES)

  if(_RULE_EMITC)
    target_compile_definitions(${_RULE_NAME}
      PUBLIC
        EMITC_IMPLEMENTATION
    )
    target_link_libraries(${_RULE_NAME}
    PRIVATE
      iree::vm::shims_emitc
    )

    list(APPEND _SRC_FILES "${_MODULE_NAME}")
  else()
    set(_GEN_EMBED_ARGS)
    list(APPEND _GEN_EMBED_ARGS "--output_header=${_GEN_H_FILE_NAME}")
    list(APPEND _GEN_EMBED_ARGS "--output_impl=${_GEN_C_FILE_NAME}")
    list(APPEND _GEN_EMBED_ARGS "--identifier=${_RULE_C_IDENTIFIER}")
    list(APPEND _GEN_EMBED_ARGS "--flatten")
    list(APPEND _GEN_EMBED_ARGS "${_MODULE_NAME}")

    add_custom_command(
      OUTPUT
        ${_GEN_H_FILE_NAME}
        ${_GEN_C_FILE_NAME}
      COMMAND generate_embed_data ${_GEN_EMBED_ARGS}
      DEPENDS generate_embed_data ${_MODULE_NAME}
    )

    list(APPEND _SRC_FILES "${_GEN_H_FILE_NAME}" "${_GEN_C_FILE_NAME}")
  endif()

  target_sources(${_RULE_NAME}
    PRIVATE
      ${_SRC_FILES}
  )

endfunction()
