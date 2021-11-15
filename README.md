# IREE Bare-Metal Arm Sample

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

You will need an installation of IREE on your host machine. The lines below can be used to build IREE from the already cloned submodule:

```shell
mkdir build-iree-host
cd build-iree-host

cmake -GNinja \
      -DCMAKE_C_COMPILER=clang \
      -DCMAKE_CXX_COMPILER=clang++ \
      -DIREE_HAL_DRIVERS_TO_BUILD="VMVX_Sync" \
      -DIREE_TARGET_BACKENDS_TO_BUILD="VMVX;DYLIB-LLVM-AOT" \
      -DIREE_BUILD_SAMPLES=OFF \
      -DIREE_BUILD_TESTS=OFF \
      -DCMAKE_INSTALL_PREFIX=../build-iree-host-install \
      ../third_party/iree/
cmake --build . --target install
cd ..
```

For further information, see the [Getting started](https://google.github.io/iree/building-from-source/getting-started/) guide.

#### Target Build

One can choose to either build the sample with libopencm3 or with CMSIS by either setting `BUILD_WITH_LIBOPENCM3` or `BUILD_WITH_CMSIS` to `ON`.
Depending on whether you build with libopencm3 or CMSIS, you need to pass the correct linker flags via `CUSTOM_ARM_LINKER_FLAGS`
and need to specify the appropriate linker script via `PATH_TO_LINKER_SCRIPT`.

```shell
mkdir build
cd build

# Set the path to the GNU Arm Embedded Toolchain, e.g.
# export PATH_TO_ARM_TOOLCHAIN="/usr/local/gcc-arm-none-eabi-10-2020-q4-major"

# To build with CMSIS
# export CUSTOM_ARM_LINKER_FLAGS="-lnosys"
# export PATH_TO_LINKER_SCRIPT="`realpath ../build_tools/stm32f4/stm32f407-cmsis-base.ld`"

# To build with libopencm3
# export CUSTOM_ARM_LINKER_FLAGS="-nostartfiles"
# export PATH_TO_LINKER_SCRIPT="`realpath ../build_tools/stm32f4/stm32f4-discovery-libopencm3-base.ld`"


# export PATH_TO_IREE_HOST_BINARY_ROOT="`realpath ../build-iree-host-install`"

cmake -GNinja \
#     -DBUILD_WITH_CMSIS=ON \
#     -DBUILD_WITH_LIBOPENCM3=ON \
      -DCMAKE_TOOLCHAIN_FILE="`realpath ../build_tools/cmake/arm.toolchain.cmake`" \
      -DARM_TOOLCHAIN_ROOT="${PATH_TO_ARM_TOOLCHAIN}" \
      -DARM_CPU="armv7e-m" \
      -DIREE_HAL_DRIVERS_TO_BUILD="VMVX_Sync" \
      -DIREE_HOST_BINARY_ROOT="${PATH_TO_IREE_HOST_BINARY_ROOT}" \
      -DCUSTOM_ARM_LINKER_FLAGS="${CUSTOM_ARM_LINKER_FLAGS}" \
      -DLINKER_SCRIPT="${PATH_TO_LINKER_SCRIPT}" \
      ..
cmake --build . --target sample_vmvx_sync
```


### Test with Renode

You can use [Renode](https://renode.io/) to execute the created binary.

```shell
wget https://github.com/renode/renode/releases/download/v1.12.0/renode-1.12.0.linux-portable.tar.gz
tar -xvzf renode-1.12.0.linux-portable.tar.gz
```
Execution is done via the interactive Renode shell (for headless execution see [here](https://github.com/renode/renode/issues/138)):

```shell
cd renode_1.12.0_portable
./renode ${PATH_TO_REPOSITORY_ROOT}/renode/stm32f4-base.resc
```
Inside the shell you need to start the execution:
```shell
(STM32F4_Discovery) start
```
You should be able to see the output of the executable in the analyzer window for uart2.

This example also comes with a robot test file:

```shell
cd renode_1.12.0_portable
test.sh --variable ELF:${PATH_TO_BINARY} ${PATH_TO_REPOSITORY_ROOT}/tests/simple_embedding.robot
```

### Custom memory configuration
Additionally a custom memory configuration exists for the STM32F4 board.
This configuration increases the size of the ROM and RAM to 256M each.

To use this custom memory configuration the target needs to be built with the appropriate linker script that accomodate the extended memory.
The build process is largely the same as before, only the linker scripts specified in ${PATH_TO_LINKER_SCRIPT} have to be amended.
The linker scripts with the custom memory configuration can be found in `build_tools/stm32f4` ending in *\*-custom.ld*.
