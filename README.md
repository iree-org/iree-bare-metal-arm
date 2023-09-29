<!--
SPDX-FileCopyrightText: 2021 Fraunhofer-Gesellschaft zur Förderung der angewandten Forschung e.V.
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->
# IREE Bare-Metal Arm Sample

[![Build and Test](https://github.com/iml130/iree-bare-metal-arm/actions/workflows/build-and-test.yml/badge.svg)](https://github.com/iml130/iree-bare-metal-arm/actions/workflows/build-and-test.yml)

**DISCLAIMER**:
This project is not intended for everyday use and made available without any support.
However, we welcome any kind of feedback via the issue tracker or if appropriate via IREE's [communication channels](https://github.com/openxla/iree#communication-channels), e.g. via the Discord server.

This projects demonstrates how to build [IREE](https://github.com/openxla/iree) with the [Arm GNU Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain) for bare-metal Arm targets using either the open-source firmware library [libopencm3](https://github.com/libopencm3/libopencm3) or [CMSIS](https://github.com/ARM-software/CMSIS_5).

## Target Support

The samples can be build for several [STM32 32-bit Arm Cortex MCUs](https://www.st.com/en/microcontrollers-microprocessors/stm32-32-bit-arm-cortex-mcus.html), for the [Nordic nRF52840 SoC](https://www.nordicsemi.com/products/nrf52840)
and for the Arm [Corstone-300](https://developer.arm.com/Processors/Corstone-300).

## Getting Started

See the [Getting Started](docs/getting_started.md) guide for instructions on how to build this project.

## Licensing

The samples are licensed under the terms of the Apache 2.0 License with LLVM Exceptions.

The repository contains further files that are licensed under other terms.
Licensing information for each file is specified via SPDX-License-Identifier tags.
