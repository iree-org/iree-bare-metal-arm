#!/bin/bash

# Copyright 2021 Fraunhofer-Gesellschaft zur Förderung der angewandten Forschung e.V.
#
# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

# Bash script to configure a build for stm32f4.

if [[ $# -ne 2 ]] ; then
  echo "Usage: $0 <lib> <device>"
  exit 1
fi

# Set linker flags
case $1 in
  cmsis)
    echo "Building with CMSIS"
    export CUSTOM_ARM_LINKER_FLAGS=""
    export BUILD_WITH="CMSIS"
    ;;
  
  crt0)
    echo "Building with crt0.s"
    export CUSTOM_ARM_LINKER_FLAGS=""
    export BUILD_WITH="CRT0"
    ;;

  libopencm3)
    echo "Building with libopencm3"
    export CUSTOM_ARM_LINKER_FLAGS="-nostartfiles"
    export BUILD_WITH="LIBOPENCM3"
    ;;

  *)
    echo "Unknown lib. Use 'cmsis', 'crt0' or 'libopencm3"
    exit 1
    ;;
esac

# Set path to linker script
case $2 in
  stm32f303xe)
    echo "Building for STM32F303xE"
    export ARM_CPU="cortex-m4"
    export PATH_TO_LINKER_SCRIPT="`realpath ../build_tools/stm32f303xe-cmsis.ld`"
    ;;

  stm32f407)
    echo "Building for STM32F407"
    export ARM_CPU="cortex-m4"
    if [ "$1" == "cmsis" ] || [ "$1" == "crt0" ]; then
      export PATH_TO_LINKER_SCRIPT="`realpath ../build_tools/stm32f407xg-cmsis.ld`"
    else
      export PATH_TO_LINKER_SCRIPT="`realpath ../build_tools/stm32f407xg-libopencm3.ld`"
    fi
    ;;
  
  stm32f446)
    echo "Building for STM32F446"
    export ARM_CPU="cortex-m4"
    if [ "$1" == "cmsis" ] || [ "$1" == "crt0" ]; then
      export PATH_TO_LINKER_SCRIPT="`realpath ../build_tools/stm32f446xe-cmsis.ld`"
    else
      export PATH_TO_LINKER_SCRIPT="`realpath ../build_tools/stm32f446xe-libopencm3.ld`"
    fi
    ;;

  stm32f4xx)
    echo "Building for STM32F4xx, high memory"
    export ARM_CPU="cortex-m4"
    if [ "$1" == "cmsis" ] || [ "$1" == "crt0" ]; then
      export PATH_TO_LINKER_SCRIPT="`realpath ../build_tools/stm32f4xxxx-cmsis.ld`"
    else
      export PATH_TO_LINKER_SCRIPT="`realpath ../build_tools/stm32f4xxxx-libopencm3.ld`"
    fi
    ;;
  
  stm32f746)
    echo "Building for STM32F746"
    export ARM_CPU="cortex-m7"
    export PATH_TO_LINKER_SCRIPT="`realpath ../build_tools/stm32f746xg-cmsis.ld`"
    ;;

  *)
    echo "Unknown device. Use 'stm32f303xe' 'stm32f407', 'stm32f446', 'stm32f4xx' or 'stm32f746'"
    exit 1
    ;;
esac

# Set the path to the GNU Arm Embedded Toolchain
if [ -z ${PATH_TO_ARM_TOOLCHAIN+x} ]; then
  export PATH_TO_ARM_TOOLCHAIN="/usr/local/gcc-arm-none-eabi-10.3-2021.10"
fi

# Set the path to the IREE host binary
export PATH_TO_IREE_HOST_BINARY_ROOT="`realpath ../build-iree-host-install`"

# Check paths
if ! [ -f "$PATH_TO_LINKER_SCRIPT" ]; then
  echo "Expected the path to linker script to be set correctly (got '$PATH_TO_LINKER_SCRIPT'): can't find linker script"
  exit 1
fi

if ! [ -d "$PATH_TO_IREE_HOST_BINARY_ROOT/bin/" ]; then
  echo "Expected the path to IREE host binary to be set correctly (got '$PATH_TO_IREE_HOST_BINARY_ROOT'): can't find bin subdirectory"
  exit 1
fi

# Configure project
cmake -GNinja \
      -DBUILD_WITH_${BUILD_WITH}=ON \
      -DCMAKE_TOOLCHAIN_FILE="`realpath ../build_tools/cmake/arm-none-eabi-gcc.cmake`" \
      -DARM_TOOLCHAIN_ROOT="${PATH_TO_ARM_TOOLCHAIN}" \
      -DARM_CPU="${ARM_CPU}" \
      -DARM_TARGET="${2^^}" \
      -DIREE_ERROR_ON_MISSING_SUBMODULES=OFF \
      -DIREE_HAL_DRIVER_DEFAULTS=OFF \
      -DIREE_HAL_DRIVER_DYLIB_SYNC=ON \
      -DIREE_HAL_DRIVER_VMVX_SYNC=ON \
      -DIREE_HOST_BINARY_ROOT="${PATH_TO_IREE_HOST_BINARY_ROOT}" \
      -DCUSTOM_ARM_LINKER_FLAGS="${CUSTOM_ARM_LINKER_FLAGS}" \
      -DLINKER_SCRIPT="${PATH_TO_LINKER_SCRIPT}" \
      ..
