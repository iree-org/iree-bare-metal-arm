// Copyright 2023 The IREE bare-metal Arm Authors
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

void usart_setup(void) {
#ifdef USE_UART0
  // Configuration for UARTE
  const nrfx_uarte_config_t uarte_config = {
      .txd_pin = NRF_GPIO_PIN_MAP(1, 1),
      .rxd_pin = NRF_GPIO_PIN_MAP(1, 0),
      .rts_pin = NRF_UARTE_PSEL_DISCONNECTED,
      .cts_pin = NRF_UARTE_PSEL_DISCONNECTED,
      .baudrate = UARTE_BAUDRATE_BAUDRATE_Baud115200,
  };

  // Enable UART
  nrfx_uarte_init(&uarte, &uarte_config, NULL);
#endif
}

void gpio_setup(void) {}

void send_blocking(uint16_t c) {
#ifdef USE_UART0
  nrfx_uarte_tx(&uarte, (uint8_t*)&c, 1, NRFX_UARTE_TX_BLOCKING);
#endif
}

bool wait_until(uint64_t nanos) { return true; }
