# Copyright 2022 Fraunhofer-Gesellschaft zur Förderung der angewandten Forschung e.V.
#
# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

# add_ihex()
#
# CMake function to add a hex file.

function(add_ihex TARGET)
  if(NOT BUILD_IHEX)
    return()
  endif()

  add_custom_command(
    TARGET ${TARGET} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} ARGS -O ihex ${TARGET}.elf ${TARGET}.hex
  )
endfunction()
