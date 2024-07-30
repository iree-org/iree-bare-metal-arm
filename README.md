<!--
SPDX-FileCopyrightText: 2021 The IREE bare-metal Arm Authors
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->
# IREE Bare-Metal Arm Sample

[![Build and Test](https://github.com/iml130/iree-bare-metal-arm/actions/workflows/build-and-test.yml/badge.svg)](https://github.com/iml130/iree-bare-metal-arm/actions/workflows/build-and-test.yml)
[![REUSE status](https://api.reuse.software/badge/github.com/iml130/iree-bare-metal-arm)](https://api.reuse.software/info/github.com/iml130/iree-bare-metal-arm)

**DISCLAIMER**:
This project is not intended for everyday use and made available without any support.
However, we welcome any kind of feedback via the issue tracker or if appropriate via IREE's [communication channels](https://github.com/iree-org/iree#communication-channels), e.g. via the Discord server.

This projects demonstrates how to build [IREE](https://github.com/iree-org/iree) with the [Arm GNU Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain) for bare-metal Arm targets using either the open-source firmware library [libopencm3](https://github.com/libopencm3/libopencm3) or [CMSIS](https://github.com/ARM-software/CMSIS_5).

The project was originally initiated at Fraunhofer IML.

## Target Support

The samples can be build for several [STM32 32-bit Arm Cortex MCUs](https://www.st.com/en/microcontrollers-microprocessors/stm32-32-bit-arm-cortex-mcus.html),
for the [Nordic nRF52840 SoC](https://www.nordicsemi.com/products/nrf52840),
for the application core of the [Nordic nRF5340 SoC](https://www.nordicsemi.com/products/nrf5340)
and for the Arm [Corstone-300](https://developer.arm.com/Processors/Corstone-300).

## Getting Started

See the [Getting Started](docs/getting_started.md) guide for instructions on how to build this project.

## Further Resources

* [Running on a bare-metal platform](https://iree.dev/guides/deployment-configurations/bare-metal/); IREE.dev Guide.
* [TinyIREE: An ML Execution Environment for Embedded Systems From Compilation to Deployment](https://doi.ieeecomputersociety.org/10.1109/MM.2022.3178068); H.-I. C. Liu, M. Brehler, M. Ravishankar, N. Vasilache, B. Vanik and S. Laurenzo; *IEEE Micro*, vol. 42, no. 5, pp. 9-16, Sept.-Oct. 2022.
* (Tiny)IREE on bare-metal Arm Platforms ([slides](https://drive.google.com/file/d/1fQHG8-r27dBrbQLSjXw9C1VgMAv9BFQg/view), [recording](https://youtu.be/xvqw4ll0oDI)); M. Brehler, L. Camphausen and S. Camphausen; OpenXLA Fall Dev Summit, October 19, 2023.

## Licensing

The samples are licensed under the terms of the Apache 2.0 License with LLVM Exceptions.

The repository contains further files that are licensed under other terms.
Licensing information for each file is specified via SPDX-License-Identifier tags.
