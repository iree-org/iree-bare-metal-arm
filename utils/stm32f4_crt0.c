// Copyright 2021 Fraunhofer-Gesellschaft zur Förderung der angewandten
//                Forschung e.V.
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

uint32_t SystemCoreClock = 16000000;

const uint32_t GPIOA_BASE = 0x40020000;
const uint32_t RCC_BASE = 0x40023800;
const uint32_t USART2_BASE = 0x40004400;

uint32_t* GPIOA_MODER = (uint32_t*)(GPIOA_BASE + 0x00);
uint32_t* GPIOA_OSPEEDR = (uint32_t*)(GPIOA_BASE + 0x08);
uint32_t* GPIOA_AFR[2] = {(uint32_t*)(GPIOA_BASE + 0x20),
                          (uint32_t*)(GPIOA_BASE + 0x24)};

uint32_t* RCC_AHB1ENR = (uint32_t*)(RCC_BASE + 0x30);
uint32_t* RCC_APB1ENR = (uint32_t*)(RCC_BASE + 0x40);

uint32_t* USART2_SR = (uint32_t*)(USART2_BASE + 0x00);
uint32_t* USART2_DR = (uint32_t*)(USART2_BASE + 0x04);
uint32_t* USART2_BRR = (uint32_t*)(USART2_BASE + 0x08);
uint32_t* USART2_CR1 = (uint32_t*)(USART2_BASE + 0x0C);

void clock_setup(void) {
  /* Enable clock for GPIOA */
  *RCC_AHB1ENR |= (1 << 0);

  /* Enable clock for USART2 */
  *RCC_APB1ENR |= (1 << 17);
}

void gpio_setup(void) {
  /* Configure alternate function mode for GPIOA */
  *GPIOA_MODER |= (2 << 4);

  /* Configure speed for GPIOA on pin 2 */
  *GPIOA_OSPEEDR |= (3 << 4);

  /* Configure alternate function mode for GPIOA on pin 2 */
  *GPIOA_AFR[0] |= (7 << 8);
}

void usart_setup(void) {
  /* Enable USART2 */
  *USART2_CR1 |= (1 << 13);

  /* Configure word length to 8bit for USART2 */
  *USART2_CR1 &= ~(1 << 12);

  /* Configure baud rate 115200 for USART2 */
  /* Computation taken over from libopencm3 */
  *USART2_BRR = (SystemCoreClock + 115200 / 2) / 115200;

  /* Enable transmitting for USART2 */
  *USART2_CR1 |= (1 << 3);
}

void usart2_send_blocking(uint8_t c) {
  *USART2_DR = c;
  while (!(*USART2_SR & (1 << 6)))
    ;
}

int _write(int file, char* ptr, int len) {
  int i;

  if (file == STDOUT_FILENO || file == STDERR_FILENO) {
    for (i = 0; i < len; i++) {
      if (ptr[i] == '\n') {
        usart2_send_blocking('\r');
      }
      usart2_send_blocking(ptr[i]);
    }
    return i;
  }
  errno = EIO;
  return -1;
}

bool wait_until(uint64_t nanos) { return true; }
