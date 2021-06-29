# IREE Bare-Metal Arm Sample

**DISCLAIMER**:
This project is not intended for everyday use and made available without any support.
However, we welcome any kind of feedback via the issue tracker or if appropriate via IREE's [communication channels](https://github.com/google/iree#communication-channels), e.g. via the Discord server.

This projects demonstrates how to build IREE with the [GNU Arm Embedded Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm) for bare-metal Arm targets.

## Getting Started

> Note:<br>
> &nbsp;&nbsp;&nbsp;&nbsp;This requires an IREE host install.

```shell
cmake -DCMAKE_TOOLCHAIN_FILE="build_tools/cmake/arm.toolchain.cmake" \
      -DARM_TOOLCHAIN_ROOT="${PATH_TO_ARM_TOOLCHAIN}" \
      -DARM_CPU="armv7e-m" \
      -DIREE_HOST_BINARY_ROOT="${PATH_TO_IREE_HOST_BINARY_ROOT}" \
      -GNinja ..
```
