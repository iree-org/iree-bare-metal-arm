// Copyright 2021 Fraunhofer-Gesellschaft zur Förderung der angewandten
//                Forschung e.V.
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <errno.h>
#include <stdint.h>
#include <unistd.h>

#include "libopencm3/stm32/gpio.h"
#include "libopencm3/stm32/rcc.h"
#include "libopencm3/stm32/usart.h"

void clock_setup(void) {
  /* Enable GPIOD clock for LED & USARTs. */
  rcc_periph_clock_enable(RCC_GPIOD);
  rcc_periph_clock_enable(RCC_GPIOA);

#ifdef USE_UART1
  /* Enable clocks for USART1. */
  rcc_periph_clock_enable(RCC_USART1);
#endif
#ifdef USE_UART2
  /* Enable clocks for USART2. */
  rcc_periph_clock_enable(RCC_USART2);
#endif
}

void usart_setup(void) {
#ifdef USE_UART1
  /* Setup USART1 parameters. */
  usart_set_baudrate(USART1, 115200);
  usart_set_databits(USART1, 8);
  usart_set_stopbits(USART1, USART_STOPBITS_1);
  usart_set_mode(USART1, USART_MODE_TX);
  usart_set_parity(USART1, USART_PARITY_NONE);
  usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);

  /* Finally enable the USART. */
  usart_enable(USART1);
#endif
#ifdef USE_UART2
  /* Setup USART2 parameters. */
  usart_set_baudrate(USART2, 115200);
  usart_set_databits(USART2, 8);
  usart_set_stopbits(USART2, USART_STOPBITS_1);
  usart_set_mode(USART2, USART_MODE_TX);
  usart_set_parity(USART2, USART_PARITY_NONE);
  usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);

  /* Finally enable the USART. */
  usart_enable(USART2);
#endif
}

void gpio_setup(void) {
  /* Setup GPIO pin GPIO12 on GPIO port D for LED. */
  gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12);

#ifdef USE_UART1
  /* Setup GPIO pins for USART1 transmit. */
  gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9);

  /* Setup USART1 TX pin as alternate function. */
  gpio_set_af(GPIOA, GPIO_AF7, GPIO9);
#endif
#ifdef USE_UART2
  /* Setup GPIO pins for USART2 transmit. */
  gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2);

  /* Setup USART2 TX pin as alternate function. */
  gpio_set_af(GPIOA, GPIO_AF7, GPIO2);
#endif
}

void send_blocking(uint16_t c) {
#ifdef USE_UART1
  usart_send_blocking(USART1, c);
#endif
#ifdef USE_UART2
  usart_send_blocking(USART2, c);
#endif
}

int _write(int file, char* ptr, int len) {
  int i;

  if (file == STDOUT_FILENO || file == STDERR_FILENO) {
    for (i = 0; i < len; i++) {
      if (ptr[i] == '\n') {
        send_blocking('\r');
      }
      send_blocking(ptr[i]);
    }
    return i;
  }
  errno = EIO;
  return -1;
}
