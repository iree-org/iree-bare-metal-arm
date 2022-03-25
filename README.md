# IREE Bare-Metal Arm Sample

[![Build and Test](https://github.com/iml130/iree-bare-metal-arm/actions/workflows/build-and-test.yml/badge.svg)](https://github.com/iml130/iree-bare-metal-arm/actions/workflows/build-and-test.yml)

**DISCLAIMER**:
This project is not intended for everyday use and made available without any support.
However, we welcome any kind of feedback via the issue tracker or if appropriate via IREE's [communication channels](https://github.com/google/iree#communication-channels), e.g. via the Discord server.

This projects demonstrates how to build [IREE](https://github.com/google/iree) with the [GNU Arm Embedded Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm) for bare-metal Arm targets using either the open-source firmware library [libopencm3](https://github.com/libopencm3/libopencm3) or [CMSIS](https://github.com/ARM-software/CMSIS_5).
## Getting Started

### Prerequisites

You need CMake and the [GNU Arm Embedded Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm) installed on your host machine. Further, a C/C++ compiler is required to build IREE for your host machine.

### Clone and Build
#### Clone

```shell
git clone https://github.com/iml130/iree-bare-metal-arm.git
cd iree-bare-metal-arm
git submodule update --init
cd third_party/iree
git submodule update --init
cd ../../
```
> Note:<br>
> &nbsp;&nbsp;&nbsp;&nbsp;The submodules used within IREE themself include submodules, so that we advice to avoid an recursive update.

> Note:<br>
> &nbsp;&nbsp;&nbsp;&nbsp;It may happen that new submodules are added to IREE.<br>
> &nbsp;&nbsp;&nbsp;&nbsp;Thus it might not be sufficient to only pull the latest master and you may need update the submodules manually.

#### Host Build

You will need an installation of IREE on your host machine.
One option is to build IREE on your host machine.
Another option is to install a snapshot build and to compile some additional tools.

##### Option A: Build IREE

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

##### Option B: Install a Snapshot

The snapshot can be installed via the following commands:

```shell
python3 -m venv venv-iree-snapshot
source venv-iree-snapshot/bin/activate
pip3 install -r requirements.txt
```

For further information, see the [TensorFlow Integration](https://google.github.io/iree/ml-frameworks/tensorflow/) or[TensorFlow Lite Integration](https://google.github.io/iree/ml-frameworks/tensorflow-lite/) guide.
The next lines can be used to build the missing tools from the already cloned submodule:

```shell
mkdir build-iree-host-tools
mkdir -p build-iree-host-install/bin
cd build-iree-host-tools

cmake -GNinja \
      -DCMAKE_C_COMPILER=clang \
      -DCMAKE_CXX_COMPILER=clang++ \
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

#### Target Build

One can choose to either build the sample with libopencm3 or with CMSIS by either setting `BUILD_WITH_LIBOPENCM3` or `BUILD_WITH_CMSIS` to `ON`.
Depending on whether you build with libopencm3 or CMSIS, you need to pass the correct linker flags via `CUSTOM_ARM_LINKER_FLAGS`
and need to specify the appropriate linker script via `PATH_TO_LINKER_SCRIPT`.
UART1 and UART2 can be initialized by setting `USE_UART1` and `USE_UART2` to `ON`.
The clock can be configured in four ways. The internal clock `HSI` or the external clock `HSE` can either be used with or without using a phase-locked loop (PLL).
To configure the clock set `CLOCK_SOURCE` to `HSI`, `HSE`, `PLL_HSI` or `PLL_HSE`. The option defaults to `HSI`.

```shell
mkdir build
cd build

# Set the path to the GNU Arm Embedded Toolchain, e.g.
# export PATH_TO_ARM_TOOLCHAIN="/usr/local/gcc-arm-none-eabi-10.3-2021.10"

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
      -DIREE_HAL_DRIVER_DEFAULTS=OFF \
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
wget https://github.com/renode/renode/releases/download/v1.12.0/renode-1.12.0.linux-portable.tar.gz
tar -xvzf renode-1.12.0.linux-portable.tar.gz
```
Execution is done via the interactive Renode shell (for headless execution see [here](https://github.com/renode/renode/issues/138)):

```shell
cd renode_1.12.0_portable
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
cd renode_1.12.0_portable
./test.sh --variable BASE_DIR:${PATH_TO_REPOSITORY_ROOT} ${PATH_TO_REPOSITORY_ROOT}/tests/vmvx_sample.robot
```
