// Copyright 2021 The IREE bare-metal Arm Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <stdbool.h>
#include <stdint.h>

#include "stm32f4xx.h"
#include "system_stm32f4xx.h"

#if defined(STM32F407xx)
#include "stm32f407xx_cmsis.h"
#elif defined(STM32F411xE)
#include "stm32f411xe_cmsis.h"
#elif defined(STM32F446xx)
#include "stm32f446xx_cmsis.h"
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
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;
#elif defined(USE_HSE_CLOCK)
  // Enable HSE clock
  // We do not explicitly set bypass mode to use the external oscillator from
  // the STLink. This should nontheless work and simplify configuration to use
  // another external oscillator (e.g. X3 on Nucleo F411RE).
  RCC->CR |= RCC_CR_HSEON;

  // Wait for HSE clock to be ready
  while (!(RCC->CR & RCC_CR_HSERDY))
    ;

  // Set system clock as HSE clock
  RCC->CFGR |= RCC_CFGR_SW_HSE;

  // Wait for system clock to be ready
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSE)
    ;

  // Enable power interface clock
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;
#elif defined(USE_PLL_HSI_CLOCK) || defined(USE_PLL_HSE_CLOCK)
#ifdef USE_PLL_HSI_CLOCK
  // Enable HSI clock
  RCC->CR |= RCC_CR_HSION;

  // Wait for HSI clock to be ready
  while (!(RCC->CR & RCC_CR_HSIRDY))
    ;
#else
  // Enable HSE clock
  // We do not explicitly set bypass mode to use the external oscillator from
  // the STLink. This should nontheless work and simplify configuration to use
  // another external oscillator (e.g. X3 on Nucleo F411RE).
  RCC->CR |= RCC_CR_HSEON;

  // Wait for HSE clock to be ready
  while (!(RCC->CR & RCC_CR_HSERDY))
    ;
#endif

  // Enable power interface clock
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;

  // Configure voltage scaling to scale 3 (higher performance but higher
  // consumption)
  PWR->CR |= PWR_CR_VOS;

  // Configure flash memory
  FLASH->ACR |=
      FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN | FLASH_WAIT_STATES;

  // Configure AHB prescaler
  RCC->CFGR |= AHB_PRESCALER;

  // Configure APB1 prescaler
  RCC->CFGR |= APB1_PRESCALER;

  // Configure APB2 prescaler
  RCC->CFGR |= APB2_PRESCALER;
#ifdef USE_PLL_HSI_CLOCK
  // Configure the PLL clock
  RCC->PLLCFGR =
      (PLL_M << 0) | (PLL_N << 6) | (PLL_P << 16) | RCC_PLLCFGR_PLLSRC_HSI;
#else
  // Configure the PLL clock
  RCC->PLLCFGR =
      (PLL_M << 0) | (PLL_N << 6) | (PLL_P << 16) | RCC_PLLCFGR_PLLSRC_HSE;
#endif

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
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
}

void usart_setup(void) {
#ifdef USE_UART1
  // Enable clock for UARt1
  RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

  // Configure GPIOA mode on pin 9 as alternate function mode
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
  //   baud_rate = (apb2_clock_frequency) / (16 * usart_div).
  // This yields
  //   usart_div = (apb2_clock_frequency) / (16 * baud_rate).
  // It is sufficent to easily write apb1_clock_frequency / baud_rate into the
  // register.
  uint32_t apb2_divider = APBPrescTable[((RCC->CFGR & RCC_CFGR_PPRE2) >> 13)];
  uint32_t apb2_clock = SystemCoreClock >> apb2_divider;
  USART1->BRR = apb2_clock / 115200;

  // Enable transmission for USART1
  USART1->CR1 |= USART_CR1_TE;

  // Enable USART1
  USART1->CR1 |= USART_CR1_UE;
#endif
#ifdef USE_UART2
  // Enable clock for UART2
  RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

  // Configure GPIOA mode on pin 2 as alternate function mode
  GPIOA->MODER |= GPIO_MODER_MODER2_1;

  // Configure GPIOA speed on pin 2 as high speed mode
  GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED2_1 | GPIO_OSPEEDR_OSPEED2_0;

  // Configure alternate function mode for GPIOA on pin 2
  GPIOA->AFR[0] |= GPIO_AFRL_AFSEL2_2 | GPIO_AFRL_AFSEL2_1 | GPIO_AFRL_AFSEL2_0;

  // Configure word length of USART2 to 8
  USART2->CR1 &= ~USART_CR1_M;

  // Configure baud rate of USART2 to 115200
  // For oversampling by 16 (default) the following equation holds (see
  // reference manual)
  //   baud_rate = (apb1_clock_frequency) / (16 * usart_div).
  // This yields
  //   usart_div = (apb1_clock_frequency) / (16 * baud_rate).
  // It is sufficent to easily write apb1_clock_frequency / baud_rate into the
  // register.
  uint32_t apb1_divider = APBPrescTable[((RCC->CFGR & RCC_CFGR_PPRE1) >> 10)];
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
  USART1->DR = c;
  while (!(USART1->SR & USART_SR_TXE))
    ;
#endif
#ifdef USE_UART2
  USART2->DR = c;
  while (!(USART2->SR & USART_SR_TXE))
    ;
#endif
}

bool wait_until(uint64_t nanos) { return true; }
