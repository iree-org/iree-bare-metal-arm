#!/bin/bash

# Copyright 2021 The IREE bare-metal Arm Authors
#
# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

# Bash script to configure a build for stm32f4.

if [[ $# -ne 2 && $# -ne 3 ]] ; then
  echo "Usage: $0 <lib> <device>"
  echo "   or: $0 <lib> <device> <build type>"
  exit 1
fi

if [[ $# -eq 3 ]] ; then
  export BUILD_TYPE=$3
else
  export BUILD_TYPE=MinSizeRel
fi

# Set the path to the GNU Arm Embedded Toolchain
if [ -z ${PATH_TO_ARM_TOOLCHAIN+x} ]; then
  export PATH_TO_ARM_TOOLCHAIN="/usr/local/arm-gnu-toolchain-13.2.Rel1-x86_64-arm-none-eabi"
fi

# Set linker flags
case $1 in
  cmsis)
    echo "Building with CMSIS"
    export CUSTOM_ARM_LINKER_FLAGS=""
    export BUILD_WITH="CMSIS"
    ;;

  libopencm3)
    echo "Building with libopencm3"
    export CUSTOM_ARM_LINKER_FLAGS="-nostartfiles"
    export BUILD_WITH="LIBOPENCM3"
    ;;

  *)
    echo "Unknown lib. Use 'cmsis' or 'libopencm3"
    exit 1
    ;;
esac

# Get path
PATH_TO_SCRIPT="`dirname $0`"
PATH_TO_REPO="`realpath ${PATH_TO_SCRIPT}/../`"

# Set path to linker script
case $2 in
  nrf52840)
    echo "Building for NRF52840"
    export ARM_CPU="cortex-m4"
    if [ -z ${PATH_TO_LINKER_SCRIPT+x} ]; then
      export PATH_TO_LINKER_SCRIPT="${PATH_TO_REPO}/third_party/nrfx-custom/nrf52840_xxaa.ld"
    fi
    ;;

  nrf5340)
    echo "Building for NRF5340"
    export ARM_CPU="cortex-m33"
    if [ -z ${PATH_TO_LINKER_SCRIPT+x} ]; then
      export PATH_TO_LINKER_SCRIPT="${PATH_TO_REPO}/third_party/nrfx-custom/nrf5340_xxaa_application.ld"
    fi
    ;;

  stm32f407)
    echo "Building for STM32F407"
    export ARM_CPU="cortex-m4"
    if [ "$1" == "cmsis" ]; then
      export PATH_TO_LINKER_SCRIPT="${PATH_TO_REPO}/build_tools/stm32f407xg-cmsis.ld"
    else
      export PATH_TO_LINKER_SCRIPT="${PATH_TO_REPO}/third_party/libopencm3-custom/stm32f407xg.ld"
    fi
    ;;

  stm32f411xe)
    echo "Building for STM32F411xE"
    export ARM_CPU="cortex-m4"
    if [ "$1" == "cmsis" ]; then
      export PATH_TO_LINKER_SCRIPT="${PATH_TO_REPO}/build_tools/stm32f446xe-cmsis.ld"
    else
      export PATH_TO_LINKER_SCRIPT="${PATH_TO_REPO}/third_party/libopencm3-custom/stm32f446xe.ld"
    fi
    ;;
  
  stm32f446)
    echo "Building for STM32F446"
    export ARM_CPU="cortex-m4"
    if [ "$1" == "cmsis" ]; then
      export PATH_TO_LINKER_SCRIPT="${PATH_TO_REPO}/build_tools/stm32f446xe-cmsis.ld"
    else
      export PATH_TO_LINKER_SCRIPT="${PATH_TO_REPO}/third_party/libopencm3-custom/stm32f446xe.ld"
    fi
    ;;

  stm32f4xx)
    echo "Building for STM32F4xx, high memory"
    export ARM_CPU="cortex-m4"
    if [ "$1" == "cmsis" ]; then
      export PATH_TO_LINKER_SCRIPT="${PATH_TO_REPO}/build_tools/stm32f4xxxx-cmsis.ld"
    else
      export PATH_TO_LINKER_SCRIPT="${PATH_TO_REPO}/third_party/libopencm3-custom/stm32f4xxxx.ld"
    fi
    ;;
  
  stm32f746)
    echo "Building for STM32F746"
    export ARM_CPU="cortex-m7"
    export PATH_TO_LINKER_SCRIPT="${PATH_TO_REPO}/build_tools/stm32f746xg-cmsis.ld"
    ;;
  
  stm32l4r5)
    echo "Building for STM32L4R5"
    export ARM_CPU="cortex-m4"
    export PATH_TO_LINKER_SCRIPT="${PATH_TO_REPO}/build_tools/stm32l4r5xi-cmsis.ld"
    ;;

  stm32l476)
    echo "Building for STM32L476"
    export ARM_CPU="cortex-m4"
    export PATH_TO_LINKER_SCRIPT="${PATH_TO_REPO}/build_tools/stm32l476xg-cmsis.ld"
    ;;

  corstone-300)
    echo "Building for Corstone-300"
    export ARM_CPU="cortex-m55"
    ${PATH_TO_ARM_TOOLCHAIN}/bin/arm-none-eabi-gcc -E -x c -P -C -o platform_parsed.ld "${PATH_TO_REPO}/third_party/ethos-u-core-platform/targets/corstone-300/platform.ld"
    export PATH_TO_LINKER_SCRIPT="`realpath platform_parsed.ld`"
    patch platform_parsed.ld ${PATH_TO_REPO}/build_tools/platform_parsed.patch
    ;;

  *)
    echo "Unknown device. Supported devices are"
    echo "  'nrf52840'"
    echo "  'nrf5340'"
    echo "  'stm32f407'"
    echo "  'stm32f411xe'"
    echo "  'stm32f446'"
    echo "  'stm32f4xx'"
    echo "  'stm32f746'"
    echo "  'stm32l4r5'"
    echo "  'stm32l476'"
    echo "  'corstone-300'"
    exit 1
    ;;
esac

# Set UART to use
case $2 in
  nrf52840)
    UART=0
    ;;

  nrf5340)
    UART=0
    ;;

  *)
    UART=2
    ;;
esac

# Set the path to the IREE host binary
export PATH_TO_IREE_HOST_BIN_DIR="`realpath ../build-iree-host-install/bin`"

# Check paths
if ! [ -f "$PATH_TO_LINKER_SCRIPT" ]; then
  echo "Expected the path to linker script to be set correctly (got '$PATH_TO_LINKER_SCRIPT'): can't find linker script"
  exit 1
fi

if ! [ -d "$PATH_TO_IREE_HOST_BIN_DIR" ]; then
  echo "Expected the path to IREE host binary to be set correctly (got '$PATH_TO_IREE_HOST_BIN_DIR'): can't find directory"
  exit 1
fi

# Configure project
cmake -GNinja \
      -DBUILD_WITH_${BUILD_WITH}=ON \
      -DCMAKE_TOOLCHAIN_FILE="${PATH_TO_REPO}/build_tools/cmake/arm-none-eabi-gcc.cmake" \
      -DARM_TOOLCHAIN_ROOT="${PATH_TO_ARM_TOOLCHAIN}" \
      -DARM_CPU="${ARM_CPU}" \
      -DARM_TARGET="${2^^}" \
      -DIREE_ERROR_ON_MISSING_SUBMODULES=OFF \
      -DIREE_HAL_DRIVER_LOCAL_SYNC=ON \
      -DIREE_HAL_EXECUTABLE_LOADER_EMBEDDED_ELF=ON \
      -DIREE_HAL_EXECUTABLE_LOADER_VMVX_MODULE=ON \
      -DIREE_HOST_BIN_DIR="${PATH_TO_IREE_HOST_BIN_DIR}" \
      -DCUSTOM_ARM_LINKER_FLAGS="${CUSTOM_ARM_LINKER_FLAGS}" \
      -DLINKER_SCRIPT="${PATH_TO_LINKER_SCRIPT}" \
      -DUSE_UART${UART}=ON \
      -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
      ..
