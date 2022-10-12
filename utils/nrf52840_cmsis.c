// Copyright 2022 Fraunhofer-Gesellschaft zur Förderung der angewandten
//                Forschung e.V.
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <stdbool.h>
#include <stdint.h>

#include "nrf_gpio.h"
#include "nrfx_uart.h"

// UART instance
#ifdef USE_UART0
nrfx_uart_t uart = NRFX_UART_INSTANCE(0);
#endif

void clock_setup(void) {}

void usart_setup(void) {
#ifdef USE_UART0
  // Configuration for UART
  const nrfx_uart_config_t uart_config = {
      .pseltxd = NRF_GPIO_PIN_MAP(0, 6),
      .pselrxd = NRF_GPIO_PIN_MAP(0, 8),
      .pselcts = NRF_UART_PSEL_DISCONNECTED,
      .pselrts = NRF_UART_PSEL_DISCONNECTED,
      .baudrate = UART_BAUDRATE_BAUDRATE_Baud115200,
  };

  // Enable UART
  nrfx_uart_init(&uart, &uart_config, NULL);
#endif
}

void gpio_setup(void) {}

void send_blocking(uint16_t c) {
#ifdef USE_UART0
  nrfx_uart_tx(&uart, (uint8_t*)&c, 1);
#endif
}

bool wait_until(uint64_t nanos) { return true; }
