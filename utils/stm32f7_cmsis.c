// Copyright 2022 Fraunhofer-Gesellschaft zur Förderung der angewandten
//                Forschung e.V.
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <stdbool.h>
#include <stdint.h>

#include "stm32f7xx.h"

void clock_setup(void) {
  /* Enable clock for GPIOA */
  // RCC->AHB1ENR |= (1<<0);
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

  /* Enable clock for USART2 */
  // RCC->APB1ENR |= (1<<17);
  RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
}

void gpio_setup(void) {
  /* Configure alternate function mode for GPIOA */
  // GPIOA->MODER |= (2<<4);
  GPIOA->MODER |= GPIO_MODER_MODER2_1;

  /* Configure speed for GPIOA on pin 2 */
  // GPIOA->OSPEEDR |= (3<<4);
  GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR2_0 | GPIO_OSPEEDR_OSPEEDR2_1;

  /* Configure alternate function mode for GPIOA on pin 2 */
  // GPIOA->AFR[0] |= (7<<8);
  GPIOA->AFR[0] |= GPIO_AFRL_AFRL2_0 | GPIO_AFRL_AFRL2_1 | GPIO_AFRL_AFRL2_2;
}

void usart_setup(void) {
  /* Enable USART2 */
  // USART2->CR1 |= (1<<13);
  USART2->CR1 |= USART_CR1_UE;

  /* Configure word length to 8bit for USART2 */
  // USART2->CR1 &= ~(1<<12);
  USART2->CR1 &= ~USART_CR1_M;

  /* Configure baud rate 115200 for USART2 */
  /* Computation taken over from libopencm3 */
  USART2->BRR = (SystemCoreClock + 115200 / 2) / 115200;

  /* Enable transmitting for USART2 */
  // USART2->CR1 |= (1<<3);
  USART2->CR1 |= USART_CR1_TE;
}

void usart_send_blocking(uint8_t c) {
  USART2->TDR = c;
  // while(!(USART2->SR & (1 << 6)));
  while (!(USART2->ISR & USART_ISR_TC))
    ;
}

bool wait_until(uint64_t nanos) { return true; }
