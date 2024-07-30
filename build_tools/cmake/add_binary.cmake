# Copyright 2022 The IREE bare-metal Arm Authors
#
# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

# add_binary()
#
# CMake function to add a binary file.

function(add_binary TARGET)
  if(NOT BUILD_BINARY)
    return()
  endif()

  add_custom_command(
    TARGET ${TARGET} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} ARGS -O binary ${TARGET}.elf ${TARGET}.bin
  )
endfunction()
