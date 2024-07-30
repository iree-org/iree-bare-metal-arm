// Copyright 2022 The IREE bare-metal Arm Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "stm32l476xx.h"

// When using the PLL, the clock needs to be configured accordingly. Adjust
// these values to your needs.

#define FLASH_WAIT_STATES FLASH_ACR_LATENCY_4WS
#define AHB_PRESCALER RCC_CFGR_HPRE_DIV1
#define APB1_PRESCALER RCC_CFGR_PPRE1_DIV1
#define APB2_PRESCALER RCC_CFGR_PPRE2_DIV1
#if defined(USE_PLL_HSI_CLOCK)
#define PLL_M 0  // divides by 1
#define PLL_N 10
#define PLL_R 0  // divides by 2
#endif
