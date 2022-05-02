# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(custom_crt0_SOURCE_DIR "${iree-bare-metal-arm_SOURCE_DIR}/third_party/custom_crt0/")

add_library(custom_crt0 STATIC "")

target_sources(custom_crt0 PRIVATE
  ${custom_crt0_SOURCE_DIR}/startup_stm32f4xx_stripped.s
)

# Allow ld to find generic linker script
target_link_directories(custom_crt0 INTERFACE "${iree-bare-metal-arm_SOURCE_DIR}/build_tools/")
