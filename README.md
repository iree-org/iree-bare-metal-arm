# IREE Bare-Metal Arm Sample

[![Build and Test](https://github.com/iml130/iree-bare-metal-arm/actions/workflows/build-and-test.yml/badge.svg)](https://github.com/iml130/iree-bare-metal-arm/actions/workflows/build-and-test.yml)

**DISCLAIMER**:
This project is not intended for everyday use and made available without any support.
However, we welcome any kind of feedback via the issue tracker or if appropriate via IREE's [communication channels](https://github.com/google/iree#communication-channels), e.g. via the Discord server.

This projects demonstrates how to build [IREE](https://github.com/google/iree) with the [Arm GNU Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain) for bare-metal Arm targets using either the open-source firmware library [libopencm3](https://github.com/libopencm3/libopencm3) or [CMSIS](https://github.com/ARM-software/CMSIS_5).
## Getting Started

### Prerequisites

You need CMake and the [Arm GNU Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain) installed on your host machine. Further, a C/C++ compiler is required to build IREE for your host machine.

### Clone and Build
#### Clone

```shell
git clone https://github.com/iml130/iree-bare-metal-arm.git
cd iree-bare-metal-arm
git submodule update --init
./build_tools/update_iree_submodules.sh
```
> Note:<br>
> &nbsp;&nbsp;&nbsp;&nbsp;The submodules used within IREE themself include submodules, so that we advice to avoid an recursive update.

> Note:<br>
> &nbsp;&nbsp;&nbsp;&nbsp;It may happen that IREE submodules need to be updated.<br>
> &nbsp;&nbsp;&nbsp;&nbsp;Thus it might not be sufficient to only pull the latest main branch.<br>
> &nbsp;&nbsp;&nbsp;&nbsp;You can (re)run [`build_tools/update_iree_submodules.sh`](https://github.com/iml130/iree-bare-metal-arm/blob/main/build_tools/update_iree_submodules.sh) to update IREE's submodules.

#### Host Build

You will need an installation of IREE on your host machine.
We recommend to install a snapshot and to only build some additional tools.
Another option is to build everything on your host machine.

##### Install a Snapshot

> TL;DR:<br>
> &nbsp;&nbsp;&nbsp;&nbsp;You can use [`build_tools/install_iree_host_tools.sh`](https://github.com/iml130/iree-bare-metal-arm/blob/main/build_tools/install_iree_host_tools.sh) to install the host build. Make sure you active the virtual environment afterwards.

The snapshot can be installed via the following commands:

```shell
python3 -m venv venv-iree-snapshot
source venv-iree-snapshot/bin/activate
pip3 install -r requirements.txt
```

For further information, see the [TensorFlow Integration](https://google.github.io/iree/getting-started/tensorflow/) or [TensorFlow Lite Integration](https://google.github.io/iree/getting-started/tflite/) guide.
The next lines can be used to build the missing tools from the already cloned submodule:

```shell
mkdir build-iree-host-tools
mkdir -p build-iree-host-install/bin
cd build-iree-host-tools

cmake -GNinja \
      -DCMAKE_C_COMPILER=clang \
      -DCMAKE_CXX_COMPILER=clang++ \
      -DIREE_ERROR_ON_MISSING_SUBMODULES=OFF \
      -DIREE_ENABLE_THREADING=OFF \
      -DIREE_HAL_DRIVER_DEFAULTS=OFF \
      -DIREE_BUILD_COMPILER=OFF \
      -DIREE_BUILD_SAMPLES=OFF \
      -DIREE_BUILD_TESTS=OFF \
      -DCMAKE_INSTALL_PREFIX=../build-iree-host-install \
      ../third_party/iree/
cmake --build . --target generate_embed_data iree-flatcc-cli

cp build_tools/embed_data/generate_embed_data ../build-iree-host-install/bin
cp build_tools/third_party/flatcc/iree-flatcc-cli ../build-iree-host-install/bin/
cd ..
```

##### Alternative: Build IREE from Source

You need to close all of IREE's submodules to build the host tools yourself.
Running `build_tools/update_iree_submodules.sh` is not sufficient. Update all submodules by running:

```shell
cd third_party/iree
git submodule update --init
cd ../../
```

The lines below can be used to build IREE from the already cloned submodule:

```shell
mkdir build-iree-host
cd build-iree-host

cmake -GNinja \
      -DCMAKE_C_COMPILER=clang \
      -DCMAKE_CXX_COMPILER=clang++ \
      -DIREE_HAL_DRIVER_DEFAULTS=OFF \
      -DIREE_HAL_DRIVER_DYLIB_SYNC=ON \
      -DIREE_HAL_DRIVER_VMVX_SYNC=ON \
      -DIREE_HAL_DRIVER_VMVX=ON \
      -DIREE_TARGET_BACKEND_DEFAULTS=OFF \
      -DIREE_TARGET_BACKEND_DYLIB_LLVM_AOT=ON \
      -DIREE_TARGET_BACKEND_VMVX=ON \
      -DIREE_BUILD_SAMPLES=OFF \
      -DIREE_BUILD_TESTS=OFF \
      -DCMAKE_INSTALL_PREFIX=../build-iree-host-install \
      ../third_party/iree/
cmake --build . --target install
cd ..
```

For further information, see the [Getting started](https://google.github.io/iree/building-from-source/getting-started/) guide.

#### Target Support

The samples can be build for several [STM32 32-bit Arm Cortex MCUs](https://www.st.com/en/microcontrollers-microprocessors/stm32-32-bit-arm-cortex-mcus.html)
and for the Arm [Corstone-300](https://developer.arm.com/Processors/Corstone-300).

##### STM32 Targets

At the moment, the samples can be build for the following [STM32 32-bit Arm Cortex MCUs](https://www.st.com/en/microcontrollers-microprocessors/stm32-32-bit-arm-cortex-mcus.html):

 * STM32F407
 * STM32F411xE
 * STM32F446
 * STM32F746
 * STM32L476

Best support is provided for boards with the
[STM32F411RE](https://www.st.com/en/microcontrollers-microprocessors/stm32f411re.html) or
[STM32F446](https://www.st.com/en/microcontrollers-microprocessors/stm32f446.html) MCU.
The samples are tested regularly on the
[STM32 Nucleo Boards](https://www.st.com/en/evaluation-tools/stm32-nucleo-boards.html)
[NUCLEO-F411RE](https://www.st.com/en/evaluation-tools/nucleo-f411re.html) and
[NUCLEO-F446RE](https://www.st.com/en/evaluation-tools/nucleo-f446re.html),
whereas other boards or rather boards with other MCUs are only supported to a limited extend.


When building for an STM32F4 MCU, one can choose to either build the samples with libopencm3 or with CMSIS by either setting `BUILD_WITH_LIBOPENCM3` or `BUILD_WITH_CMSIS` to `ON`.
Depending on whether you build with libopencm3 or CMSIS, you need to pass the correct linker flags via `CUSTOM_ARM_LINKER_FLAGS`
and need to specify the appropriate linker script via `PATH_TO_LINKER_SCRIPT`.


When using CMSIS and building for STM32411xE or STM32F446, one can select with UART to use.
UART1 and UART2 can be initialized by setting `USE_UART1` and `USE_UART2` to `ON`.
For STM32F407 and STM32F746 only UART2 is supported.


When using CMSIS and building for one of the supported STM32F4 MCUs, the clock can be configured in four ways.
The internal clock `HSI` or the external clock `HSE` can either be used with or without using a phase-locked loop (PLL).
To configure the clock set `CLOCK_SOURCE` to `HSI`, `HSE`, `PLL_HSI` or `PLL_HSE`. The option defaults to `HSI`.

When using CMSIS and building for STM32L476, the clock can be configured in two ways.
The internal clock `HSI` can either be used with or without using a phase-locked loop (PLL).
To configure the clock set `CLOCK_SOURCE` to `HSI` or `PLL_HSI`. The option defaults to `HSI`. The options `HSE` and `PLL_HSE` are not supported since the [NUCLEO-L476RG](https://www.st.com/en/evaluation-tools/nucleo-l476rg.html) board does not allow to use the clock of the attached ST-LINK as the external clock by default. The STM32L476 also has an internal multi-speed clock `MSI` which is not supported.

#### Corstone-300 Target

The Corstone-300 target is only supported with CMSIS.

#### Target Build

To build for the desired target specify the `ARM_TARGET` (as listed above) and set `ARM_CPU` accordingly.
The toolchain file supports building for the CPUs `cortex-m4`, `cortex-m7-sp` (equivalent to `cortex-m7`), `cortex-m7-dp` and `cortex-m55`.

```shell
mkdir build
cd build

# Set the path to the GNU Arm Embedded Toolchain, e.g.
# export PATH_TO_ARM_TOOLCHAIN="/usr/local/gcc-arm-11.2-2022.02-x86_64-arm-none-eabi"

# To build with CMSIS
# export CUSTOM_ARM_LINKER_FLAGS=""
# export PATH_TO_LINKER_SCRIPT="`realpath ../build_tools/stm32f407xg-cmsis.ld`"

# To build with libopencm3
# export CUSTOM_ARM_LINKER_FLAGS="-nostartfiles"
# export PATH_TO_LINKER_SCRIPT="`realpath ../build_tools/stm32f407xg-libopencm3.ld`"


# export PATH_TO_IREE_HOST_BINARY_ROOT="`realpath ../build-iree-host-install`"

cmake -GNinja \
#     -DBUILD_WITH_CMSIS=ON \
#     -DBUILD_WITH_LIBOPENCM3=ON \
      -DCMAKE_TOOLCHAIN_FILE="`realpath ../build_tools/cmake/arm-none-eabi-gcc.cmake`" \
      -DARM_TOOLCHAIN_ROOT="${PATH_TO_ARM_TOOLCHAIN}" \
      -DARM_CPU="cortex-m4" \
      -DARM_TARGET="STM32F407" \
      -DIREE_HAL_DRIVER_DYLIB_SYNC=ON \
      -DIREE_HAL_DRIVER_VMVX_SYNC=ON \
      -DIREE_HOST_BINARY_ROOT="${PATH_TO_IREE_HOST_BINARY_ROOT}" \
      -DCUSTOM_ARM_LINKER_FLAGS="${CUSTOM_ARM_LINKER_FLAGS}" \
      -DLINKER_SCRIPT="${PATH_TO_LINKER_SCRIPT}" \
#     -DUSE_UART1=ON \
#     -DUSE_UART2=ON \
      ..
cmake --build . --target sample_vmvx_sync
```
> Note:<br>
> &nbsp;&nbsp;&nbsp;&nbsp;You can use the [`build_tools/configure_build.sh`](https://github.com/iml130/iree-bare-metal-arm/blob/main/build_tools/configure_build.sh) shell script to configure the build.

### Test with Renode

You can use [Renode](https://renode.io/) to execute the created binary.

```shell
wget https://github.com/renode/renode/releases/download/v1.13.0/renode-1.13.0.linux-portable.tar.gz
tar -xvzf renode-1.13.0.linux-portable.tar.gz
```
Execution is done via the interactive Renode shell (for headless execution see [here](https://github.com/renode/renode/issues/138)):

```shell
cd renode_1.13.0_portable
./renode
```
Inside the shell you need to execute the following statements:
```shell
(monitor) include @${PATH_TO_REPOSITORY_ROOT}/third_party/renode/stm32f407.resc
(STM32F407) sysbus LoadELF @${PATH_TO_BINARY}
(STM32F407) start
```
You should be able to see the output of the executable in the analyzer window for uart2.

This example also comes with a robot test file:

```shell
cd renode_1.13.0_portable
./test.sh --variable BASE_DIR:${PATH_TO_REPOSITORY_ROOT} ${PATH_TO_REPOSITORY_ROOT}/tests/vmvx_sample.robot
```

### Test with a Fixed Virtual Platform

The samples build for Corstone-300 can also be tested with an Arm Ecosystem Fixed Virtual Platform (FVP).
The Corstone-300 Ecosysten FVP can be downloaded from the [Arm Ecosystem FVP download page](https://developer.arm.com/downloads/-/arm-ecosystem-fvps).
