# IREE Bare-Metal Arm Sample

**DISCLAIMER**:
This project is not intended for everyday use and made available without any support.
However, we welcome any kind of feedback via the issue tracker or if appropriate via IREE's [communication channels](https://github.com/google/iree#communication-channels), e.g. via the Discord server.

This projects demonstrates how to build IREE with the [GNU Arm Embedded Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm) for bare-metal Arm targets.

This projects demonstrates how to build [IREE](https://github.com/google/iree) with the [GNU Arm Embedded Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm) for bare-metal Arm targets using the open-source firmware library [libopencm3](https://github.com/libopencm3/libopencm3).
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
      -DIREE_HAL_DRIVERS_TO_BUILD="VMVX" \
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

```shell
mkdir build
cd build

# export PATH_TO_IREE_HOST_BINARY_ROOT="`realpath ../build-iree-host-install`"
# export PATH_TO_LINKER_SCRIPT="`realpath ../build_tools/stm32f4-discovery.ld`"

cmake -GNinja \
      -DCMAKE_TOOLCHAIN_FILE="build_tools/cmake/arm.toolchain.cmake" \
      -DARM_TOOLCHAIN_ROOT="${PATH_TO_ARM_TOOLCHAIN}" \
      -DARM_CPU="armv7e-m" \
      -DIREE_HOST_BINARY_ROOT="${PATH_TO_IREE_HOST_BINARY_ROOT}" \
      -DLINKER_SCRIPT="${PATH_TO_LINKER_SCRIPT}" \
      ..
cmake --build . --target simple_embedding
```
