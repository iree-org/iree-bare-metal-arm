# Copyright 2023 Fraunhofer-Gesellschaft zur Förderung der angewandten Forschung e.V.
#
# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

# vmvx_module()
#
# CMake function to create a VMVX module.

function(vmvx_module)
  cmake_parse_arguments(
    _RULE
    "INLINE_HAL"
    "NAME;SRC;C_IDENTIFIER"
    "FLAGS"
    ${ARGN}
  )

set(_COMPILE_ARGS ${_RULE_FLAGS})
list(APPEND _COMPILE_ARGS "--output-format=vm-bytecode")
if (${_RULE_INLINE_HAL})
  list(APPEND _COMPILE_ARGS "--iree-execution-model=inline-static")
  list(APPEND _COMPILE_ARGS "--iree-hal-target-backends=vmvx-inline")
else()
  list(APPEND _COMPILE_ARGS "--iree-hal-target-backends=vmvx")
endif()
list(APPEND _COMPILE_ARGS "${_RULE_SRC}")
list(APPEND _COMPILE_ARGS "-o")
list(APPEND _COMPILE_ARGS "${_RULE_NAME}.vmfb")

add_custom_command(
  OUTPUT
    ${_RULE_NAME}.vmfb
  COMMAND ${_COMPILE_TOOL_EXECUTABLE} ${_COMPILE_ARGS}
)

set(_GEN_EMBED_ARGS)
list(APPEND _GEN_EMBED_ARGS "--output_header=${_RULE_NAME}.h")
list(APPEND _GEN_EMBED_ARGS "--output_impl=${_RULE_NAME}.c")
list(APPEND _GEN_EMBED_ARGS "--identifier=${_RULE_C_IDENTIFIER}")
list(APPEND _GEN_EMBED_ARGS "--flatten")
list(APPEND _GEN_EMBED_ARGS "${_RULE_NAME}.vmfb")

add_custom_command(
  OUTPUT
    ${_RULE_NAME}.h
    ${_RULE_NAME}.c
  COMMAND iree-c-embed-data ${_GEN_EMBED_ARGS}
  DEPENDS iree-c-embed-data ${_RULE_NAME}.vmfb
)

add_library(${_RULE_NAME} STATIC "")
target_sources(${_RULE_NAME}
  PRIVATE
    ${_RULE_NAME}.c
    ${_RULE_NAME}.h
)

endfunction()
