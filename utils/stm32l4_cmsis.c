// Copyright 2021 Fraunhofer-Gesellschaft zur Förderung der angewandten
//                Forschung e.V.
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#include "stm32l4xx.h"
#include "system_stm32l4xx.h"

#if defined(STM32L4R5xx)
#include "stm32l4r5xx_cmsis.h"
#elif defined(STM32L476xx)
#include "stm32l476xx_cmsis.h"
#endif

void clock_setup(void) {
#if defined(USE_HSI_CLOCK)
  // Enable HSI clock
  RCC->CR |= RCC_CR_HSION;

  // Wait for HSI clock to be ready
  while (!(RCC->CR & RCC_CR_HSIRDY))
    ;

  // Set system clock as HSI clock
  RCC->CFGR |= RCC_CFGR_SW_HSI;

  // Wait for system clock to be ready
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI)
    ;

  // Enable power interface clock
  RCC->APB1ENR1 |= RCC_APB1ENR1_PWREN;
#elif defined(USE_PLL_HSI_CLOCK)
  // Enable HSI clock
  RCC->CR |= RCC_CR_HSION;

  // Wait for HSI clock to be ready
  while (!(RCC->CR & RCC_CR_HSIRDY))
    ;

  // Enable power interface clock
  RCC->APB1ENR1 |= RCC_APB1ENR1_PWREN;

  // Configure voltage scaling to scale 3 (higher performance but higher
  // consumption)
  PWR->CR1 |= PWR_CR1_VOS;

  // Configure flash memory
  FLASH->ACR |=
      FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN | FLASH_WAIT_STATES;

  // Configure AHB prescaler
  RCC->CFGR |= AHB_PRESCALER;

  // Configure APB1 prescaler
  RCC->CFGR |= APB1_PRESCALER;

  // Configure APB2 prescaler
  RCC->CFGR |= APB2_PRESCALER;

  // Configure the PLL clock
  RCC->PLLCFGR = (PLL_M << 4) | (PLL_N << 8) | (PLL_R << 25) |
                 RCC_PLLCFGR_PLLREN | RCC_PLLCFGR_PLLSRC_HSI;

  // Enable PLL clock
  RCC->CR |= RCC_CR_PLLON;

  // Wait for PLL clock to be ready
  while (!(RCC->CR & RCC_CR_PLLRDY))
    ;

  // Set system clock as PLL clock
  RCC->CFGR |= RCC_CFGR_SW_PLL;

  // Wait for system clock to be ready
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
    ;
#endif

  // Update CMSIS SystemCoreClock
  SystemCoreClockUpdate();
}

void gpio_setup(void) {
  // Enable clock for GPIOA
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
}

void usart_setup(void) {
#ifdef USE_UART1
  // Enable clock for UART1
  RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

  // Configure GPIOA mode on pin 9 as alternate function mode
  GPIOA->MODER &= ~GPIO_MODER_MODE9_Msk;
  GPIOA->MODER |= GPIO_MODER_MODER9_1;

  // Configure GPIOA speed on pin 9 as high speed mode
  GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED9_1 | GPIO_OSPEEDR_OSPEED9_0;

  // Configure alternate function mode for GPIOA on pin 9
  GPIOA->AFR[1] |= GPIO_AFRH_AFSEL9_2 | GPIO_AFRH_AFSEL9_1 | GPIO_AFRH_AFSEL9_0;

  // Configure word length of USART1 to 8
  USART1->CR1 &= ~USART_CR1_M;

  // Configure baud rate of USART1 to 115200
  // For oversampling by 16 (default) the following equation holds (see
  // reference manual)
  //   baud_rate = apb2_clock_frequency / usart_div.
  // This yields
  //   usart_div = apb2_clock_frequency / baud_rate.
  uint32_t apb2_divider =
      APBPrescTable[((RCC->CFGR & RCC_CFGR_PPRE2) >> RCC_CFGR_PPRE2_Pos)];
  uint32_t apb2_clock = SystemCoreClock >> apb2_divider;
  USART1->BRR = apb2_clock / 115200;

  // Enable transmission for USART1
  USART1->CR1 |= USART_CR1_TE;

  // Enable USART1
  USART1->CR1 |= USART_CR1_UE;
#endif
#ifdef USE_UART2
  // Enable clock for UART2
  RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;

  // Configure GPIOA mode on pin 2 as alternate function mode
  GPIOA->MODER &= ~GPIO_MODER_MODE2_Msk;
  GPIOA->MODER |= GPIO_MODER_MODER2_1;

  // Configure GPIOA type as output push pull
  // GPIOA->OTYPER &= ~GPIO_OTYPER_OT2_Msk;

  // Configure GPIOA speed on pin 2 as high speed mode
  // GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED2_Msk;
  GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED2_1 | GPIO_OSPEEDR_OSPEED2_0;

  // Configure alternate function mode for GPIOA on pin 2
  // GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL2_Msk;
  GPIOA->AFR[0] |= GPIO_AFRL_AFSEL2_2 | GPIO_AFRL_AFSEL2_1 | GPIO_AFRL_AFSEL2_0;

  // Configure word length of USART2 to 8
  USART2->CR1 &= ~USART_CR1_M;

  // Configure baud rate of USART2 to 115200
  // For oversampling by 16 (default) the following equation holds (see
  // reference manual)
  //   baud_rate = apb2_clock_frequency / usart_div.
  // This yields
  //   usart_div = apb2_clock_frequency / baud_rate.
  uint32_t apb1_divider =
      APBPrescTable[((RCC->CFGR & RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos)];
  uint32_t apb1_clock = SystemCoreClock >> apb1_divider;
  USART2->BRR = apb1_clock / 115200;

  // Enable transmission for USART2
  USART2->CR1 |= USART_CR1_TE;

  // Enable USART2
  USART2->CR1 |= USART_CR1_UE;
#endif
}

void usart_send_blocking(uint8_t c) {
#ifdef USE_UART1
  USART1->TDR = c;
  while (!(USART1->ISR & USART_ISR_TXE))
    ;
#endif
#ifdef USE_UART2
  USART2->TDR = c;
  while (!(USART2->ISR & USART_ISR_TXE))
    ;
#endif
}

bool wait_until(uint64_t nanos) { return true; }
