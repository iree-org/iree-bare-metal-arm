// Copyright 2021 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// A example of setting up the HAL module to run simple pointwise array
// multiplication with the device implemented by different backends via
// create_sample_driver().

#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#ifdef BUILD_WITH_CMSIS
#include <stm32f4xx.h>
#endif
#ifdef BUILD_WITH_CRT0
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
#endif
#ifdef BUILD_WITH_LIBOPENCM3
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#endif

#include "iree/base/api.h"
#include "iree/hal/api.h"
#include "iree/modules/hal/module.h"
#include "iree/vm/api.h"
#include "iree/vm/bytecode_module.h"

// A function to create the HAL device from the different backend targets.
// The HAL device is returned based on the implementation, and it must be
// released by the caller.
extern iree_status_t create_sample_device(iree_allocator_t host_allocator,
                                          iree_hal_device_t** out_device);

// A function to load the vm bytecode module from the different backend targets.
// The bytecode module is generated for the specific backend and platform.
extern const iree_const_byte_span_t load_bytecode_module_data();

iree_status_t Run() {
  // TODO(benvanik): move to instance-based registration.
  IREE_RETURN_IF_ERROR(iree_hal_module_register_types());

  iree_vm_instance_t* instance = NULL;
  IREE_RETURN_IF_ERROR(
      iree_vm_instance_create(iree_allocator_system(), &instance));

  iree_hal_device_t* device = NULL;
  IREE_RETURN_IF_ERROR(create_sample_device(iree_allocator_system(), &device),
                       "create device");
  iree_vm_module_t* hal_module = NULL;
  IREE_RETURN_IF_ERROR(
      iree_hal_module_create(device, iree_allocator_system(), &hal_module));

  // Load bytecode module from the embedded data.
  const iree_const_byte_span_t module_data = load_bytecode_module_data();

  iree_vm_module_t* bytecode_module = NULL;
  IREE_RETURN_IF_ERROR(iree_vm_bytecode_module_create(
      module_data, iree_allocator_null(), iree_allocator_system(),
      &bytecode_module));

  // Allocate a context that will hold the module state across invocations.
  iree_vm_context_t* context = NULL;
  iree_vm_module_t* modules[] = {hal_module, bytecode_module};
  IREE_RETURN_IF_ERROR(iree_vm_context_create_with_modules(
      instance, IREE_VM_CONTEXT_FLAG_NONE, &modules[0], IREE_ARRAYSIZE(modules),
      iree_allocator_system(), &context));
  iree_vm_module_release(hal_module);
  iree_vm_module_release(bytecode_module);

  // Lookup the entry point function.
  // Note that we use the synchronous variant which operates on pure type/shape
  // erased buffers.
  const char kMainFunctionName[] = "module.simple_mul";
  iree_vm_function_t main_function;
  IREE_RETURN_IF_ERROR(iree_vm_context_resolve_function(
      context, iree_make_cstring_view(kMainFunctionName), &main_function));

  // Initial buffer contents for 4 * 2 = 8.
  const float kFloat4[] = {4.0f, 4.0f, 4.0f, 4.0f};
  const float kFloat2[] = {2.0f, 2.0f, 2.0f, 2.0f};

  // Allocate buffers in device-local memory so that if the device has an
  // independent address space they live on the fast side of the fence.
  iree_hal_dim_t shape[1] = {IREE_ARRAYSIZE(kFloat4)};
  iree_hal_buffer_view_t* arg0_buffer_view = NULL;
  iree_hal_buffer_view_t* arg1_buffer_view = NULL;
  IREE_RETURN_IF_ERROR(iree_hal_buffer_view_allocate_buffer(
      iree_hal_device_allocator(device), shape, IREE_ARRAYSIZE(shape),
      IREE_HAL_ELEMENT_TYPE_FLOAT_32, IREE_HAL_ENCODING_TYPE_DENSE_ROW_MAJOR,
      IREE_HAL_MEMORY_TYPE_DEVICE_LOCAL | IREE_HAL_MEMORY_TYPE_HOST_VISIBLE,
      IREE_HAL_BUFFER_USAGE_DISPATCH | IREE_HAL_BUFFER_USAGE_TRANSFER |
          IREE_HAL_BUFFER_USAGE_MAPPING,
      iree_make_const_byte_span(kFloat4, sizeof(kFloat4)), &arg0_buffer_view));
  IREE_RETURN_IF_ERROR(iree_hal_buffer_view_allocate_buffer(
      iree_hal_device_allocator(device), shape, IREE_ARRAYSIZE(shape),
      IREE_HAL_ELEMENT_TYPE_FLOAT_32, IREE_HAL_ENCODING_TYPE_DENSE_ROW_MAJOR,
      IREE_HAL_MEMORY_TYPE_DEVICE_LOCAL | IREE_HAL_MEMORY_TYPE_HOST_VISIBLE,
      IREE_HAL_BUFFER_USAGE_DISPATCH | IREE_HAL_BUFFER_USAGE_TRANSFER |
          IREE_HAL_BUFFER_USAGE_MAPPING,
      iree_make_const_byte_span(kFloat2, sizeof(kFloat2)), &arg1_buffer_view));

  // Setup call inputs with our buffers.
  iree_vm_list_t* inputs = NULL;
  IREE_RETURN_IF_ERROR(iree_vm_list_create(
                           /*element_type=*/NULL,
                           /*capacity=*/2, iree_allocator_system(), &inputs),
                       "can't allocate input vm list");

  iree_vm_ref_t arg0_buffer_view_ref =
      iree_hal_buffer_view_move_ref(arg0_buffer_view);
  iree_vm_ref_t arg1_buffer_view_ref =
      iree_hal_buffer_view_move_ref(arg1_buffer_view);
  IREE_RETURN_IF_ERROR(
      iree_vm_list_push_ref_move(inputs, &arg0_buffer_view_ref));
  IREE_RETURN_IF_ERROR(
      iree_vm_list_push_ref_move(inputs, &arg1_buffer_view_ref));

  // Prepare outputs list to accept the results from the invocation.
  // The output vm list is allocated statically.
  iree_vm_list_t* outputs = NULL;
  IREE_RETURN_IF_ERROR(iree_vm_list_create(
                           /*element_type=*/NULL,
                           /*capacity=*/1, iree_allocator_system(), &outputs),
                       "can't allocate output vm list");

  // Synchronously invoke the function.
  IREE_RETURN_IF_ERROR(iree_vm_invoke(
      context, main_function, IREE_VM_INVOCATION_FLAG_NONE,
      /*policy=*/NULL, inputs, outputs, iree_allocator_system()));

  // Get the result buffers from the invocation.
  iree_hal_buffer_view_t* ret_buffer_view =
      (iree_hal_buffer_view_t*)iree_vm_list_get_ref_deref(
          outputs, 0, iree_hal_buffer_view_get_descriptor());
  if (ret_buffer_view == NULL) {
    return iree_make_status(IREE_STATUS_NOT_FOUND,
                            "can't find return buffer view");
  }

  // Read back the results and ensure we got the right values.
  float results[] = {0.0f, 0.0f, 0.0f, 0.0f};
  IREE_RETURN_IF_ERROR(
      iree_hal_buffer_read_data(iree_hal_buffer_view_buffer(ret_buffer_view), 0,
                                results, sizeof(results)));
  for (iree_host_size_t i = 0; i < IREE_ARRAYSIZE(results); ++i) {
    if (results[i] != 8.0f) {
      return iree_make_status(IREE_STATUS_UNKNOWN, "result mismatches");
    }
  }

  iree_vm_list_release(inputs);
  iree_vm_list_release(outputs);
  iree_hal_device_release(device);
  iree_vm_context_release(context);
  iree_vm_instance_release(instance);
  return iree_ok_status();
}

#ifdef BUILD_WITH_CMSIS
static void clock_setup(void) {
  /* Enable clock for GPIOA */
  // RCC->AHB1ENR |= (1<<0);
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

  /* Enable clock for USART2 */
  // RCC->APB1ENR |= (1<<17);
  RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
}

static void gpio_setup(void) {
  /* Configure alternate function mode for GPIOA */
  // GPIOA->MODER |= (2<<4);
  GPIOA->MODER |= GPIO_MODER_MODER2_1;

  /* Configure speed for GPIOA on pin 2 */
  // GPIOA->OSPEEDR |= (3<<4);
  GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED2_0 | GPIO_OSPEEDR_OSPEED2_1;

  /* Configure alternate function mode for GPIOA on pin 2 */
  // GPIOA->AFR[0] |= (7<<8);
  GPIOA->AFR[0] |= GPIO_AFRL_AFSEL2_0 | GPIO_AFRL_AFSEL2_1 | GPIO_AFRL_AFSEL2_2;
}

static void usart_setup(void) {
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

void usart2_send_blocking(uint8_t c) {
  USART2->DR = c;
  // while(!(USART2->SR & (1 << 6)));
  while (!(USART2->SR & USART_SR_TC))
    ;
}
#endif

#ifdef BUILD_WITH_CRT0
static void clock_setup(void) {
  /* Enable clock for GPIOA */
  *RCC_AHB1ENR |= (1 << 0);

  /* Enable clock for USART2 */
  *RCC_APB1ENR |= (1 << 17);
}

static void gpio_setup(void) {
  /* Configure alternate function mode for GPIOA */
  *GPIOA_MODER |= (2 << 4);

  /* Configure speed for GPIOA on pin 2 */
  *GPIOA_OSPEEDR |= (3 << 4);

  /* Configure alternate function mode for GPIOA on pin 2 */
  *GPIOA_AFR[0] |= (7 << 8);
}

static void usart_setup(void) {
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
#endif

#ifdef BUILD_WITH_LIBOPENCM3
static void clock_setup(void) {
  /* Enable GPIOD clock for LED & USARTs. */
  rcc_periph_clock_enable(RCC_GPIOD);
  rcc_periph_clock_enable(RCC_GPIOA);

  /* Enable clocks for USART2. */
  rcc_periph_clock_enable(RCC_USART2);
}

static void usart_setup(void) {
  /* Setup USART2 parameters. */
  usart_set_baudrate(USART2, 115200);
  usart_set_databits(USART2, 8);
  usart_set_stopbits(USART2, USART_STOPBITS_1);
  usart_set_mode(USART2, USART_MODE_TX);
  usart_set_parity(USART2, USART_PARITY_NONE);
  usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);

  /* Finally enable the USART. */
  usart_enable(USART2);
}

static void gpio_setup(void) {
  /* Setup GPIO pin GPIO12 on GPIO port D for LED. */
  gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12);

  /* Setup GPIO pins for USART2 transmit. */
  gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2);

  /* Setup USART2 TX pin as alternate function. */
  gpio_set_af(GPIOA, GPIO_AF7, GPIO2);
}
#endif

int _write(int file, char* ptr, int len) {
  int i;

  if (file == STDOUT_FILENO || file == STDERR_FILENO) {
    for (i = 0; i < len; i++) {
      if (ptr[i] == '\n') {
#if defined BUILD_WITH_CMSIS || defined BUILD_WITH_CRT0
        usart2_send_blocking('\r');
#endif
#ifdef BUILD_WITH_LIBOPENCM3
        usart_send_blocking(USART2, '\r');
#endif
      }
#if defined BUILD_WITH_CMSIS || defined BUILD_WITH_CRT0
      usart2_send_blocking(ptr[i]);
#endif
#ifdef BUILD_WITH_LIBOPENCM3
      usart_send_blocking(USART2, ptr[i]);
#endif
    }
    return i;
  }
  errno = EIO;
  return -1;
}

int main(void) {
  clock_setup();
  gpio_setup();
  usart_setup();

  printf("Running simple_embedding...\n");

  const iree_status_t result = Run();
  int ret = (int)iree_status_code(result);
  if (!iree_status_is_ok(result)) {
    iree_status_fprint(stderr, result);
    iree_status_free(result);
  } else {
    printf("Execution succesfull!\n");
  }
  printf("simple_embedding done\n");

  while (1) {
  }

  return 0;
}
