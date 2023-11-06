// Copyright 2023 Fraunhofer-Gesellschaft zur Förderung der angewandten Forschung e.V.
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <stdbool.h>
#include <stdint.h>

#include "nrf_gpio.h"
#include "nrfx_uarte.h"

// UART instance
#ifdef USE_UART0
nrfx_uarte_t uarte = NRFX_UARTE_INSTANCE(0);
#endif

void clock_setup(void) {}

// TODO: Fix me!
void usart_setup(void) {
#ifdef USE_UART0
  // Configuration for UARTE
  const nrfx_uarte_config_t uarte_config = {
      .pseltxd = NRF_GPIO_PIN_MAP(0, 6),
      .pselrxd = NRF_GPIO_PIN_MAP(0, 8),
      .pselcts = NRF_UARTE_PSEL_DISCONNECTED,
      .pselrts = NRF_UARTE_PSEL_DISCONNECTED,
      .baudrate = UARTE_BAUDRATE_BAUDRATE_Baud115200,
  };

  // Enable UART
  nrfx_uarte_init(&uarte, &uarte_config, NULL);
#endif
}

void gpio_setup(void) {}

void send_blocking(uint16_t c) {
#ifdef USE_UART0
  nrfx_uarte_tx(&uarte, (uint8_t*)&c, 1);
#endif
}

bool wait_until(uint64_t nanos) { return true; }
