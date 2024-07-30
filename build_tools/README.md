<!--
SPDX-FileCopyrightText: 2021 The IREE bare-metal Arm Authors
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->
# Build Tools

The `stm32-cmsis-base.ld` and `stm32f407xg-cmsis.ld` linker scripts are based on the the [`gcc_arm.ld`](https://github.com/ARM-software/CMSIS_5/blob/ca1b514243d8e69f1a8190e59de4b0c4ea6bdcaa/Device/_Template_Vendor/Vendor/Device/Source/GCC/gcc_arm.ld) linker script from the [ARM-software/CMSIS_5](https://github.com/ARM-software/CMSIS_5) repository. The `gcc_arm.ld` linker script was split, such that common parts are in `stm32-cmsis-base.ld` whereas the memory layout is defined in `stm32f407xg-cmsis.ld`.
The `stm32f4xxxx-cmsis.ld` is a linker script adopted to a custom memory configuration.
