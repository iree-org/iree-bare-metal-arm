// Copyright 2021 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// A example of static library loading in IREE. See the README.md for more info.
// Note: this demo requires artifacts from iree-translate before it will run.

#include <errno.h>
#ifdef BUILD_WITH_CMSIS
#include <stm32f4xx.h>
#endif
#ifdef BUILD_WITH_LIBOPENCM3
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#endif
#include <stdio.h>
#include <unistd.h>

#include "iree/hal/local/loaders/static_library_loader.h"
#include "iree/hal/local/sync_device.h"
#include "iree/modules/hal/module.h"
#include "iree/runtime/api.h"

extern const iree_hal_executable_library_header_t**
simple_mul_dispatch_0_library_query(
    iree_hal_executable_library_version_t max_version, void* reserved);
// A function to create the bytecode or C module.
extern iree_status_t create_module(iree_vm_module_t** module);

extern void print_success();

// A function to create the HAL device from the different backend targets.
// The HAL device is returned based on the implementation, and it must be
// released by the caller.
iree_status_t create_device_with_static_loader(iree_allocator_t host_allocator,
                                               iree_hal_device_t** out_device) {
  iree_status_t status = iree_ok_status();

  // Set paramters for the device created in the next step.
  iree_hal_sync_device_params_t params;
  iree_hal_sync_device_params_initialize(&params);

  // Load the statically embedded library
  const iree_hal_executable_library_header_t** static_library =
      simple_mul_dispatch_0_library_query(
          IREE_HAL_EXECUTABLE_LIBRARY_LATEST_VERSION, /*reserved=*/NULL);
  const iree_hal_executable_library_header_t** libraries[1] = {static_library};

  iree_hal_executable_loader_t* library_loader = NULL;
  if (iree_status_is_ok(status)) {
    status = iree_hal_static_library_loader_create(
        IREE_ARRAYSIZE(libraries), libraries,
        iree_hal_executable_import_provider_null(), host_allocator,
        &library_loader);
  }

  // Use the default host allocator for buffer allocations.
  iree_string_view_t identifier = iree_make_cstring_view("sync");
  iree_hal_allocator_t* device_allocator = NULL;
  if (iree_status_is_ok(status)) {
    status = iree_hal_allocator_create_heap(identifier, host_allocator,
                                            host_allocator, &device_allocator);
  }

  // Create the device and release the executor and loader afterwards.
  if (iree_status_is_ok(status)) {
    status = iree_hal_sync_device_create(
        identifier, &params, /*loader_count=*/1, &library_loader,
        device_allocator, host_allocator, out_device);
  }

  iree_hal_allocator_release(device_allocator);
  iree_hal_executable_loader_release(library_loader);
  return status;
}

iree_status_t Run() {
  iree_status_t status = iree_ok_status();

  // Instance configuration (this should be shared across sessions).
  iree_runtime_instance_options_t instance_options;
  iree_runtime_instance_options_initialize(IREE_API_VERSION_LATEST,
                                           &instance_options);
  iree_runtime_instance_options_use_all_available_drivers(&instance_options);
  iree_runtime_instance_t* instance = NULL;

  if (iree_status_is_ok(status)) {
    status = iree_runtime_instance_create(&instance_options,
                                          iree_allocator_system(), &instance);
  }

  // Create dylib device with static loader.
  iree_hal_device_t* device = NULL;
  if (iree_status_is_ok(status)) {
    status = create_device_with_static_loader(iree_allocator_system(), &device);
  }

  // Session configuration (one per loaded module to hold module state).
  iree_runtime_session_options_t session_options;
  iree_runtime_session_options_initialize(&session_options);
  iree_runtime_session_t* session = NULL;
  if (iree_status_is_ok(status)) {
    status = iree_runtime_session_create_with_device(
        instance, &session_options, device,
        iree_runtime_instance_host_allocator(instance), &session);
  }

  // Load bytecode module from the embedded data. Append to the session.
  iree_vm_module_t* module = NULL;

  if (iree_status_is_ok(status)) {
    status = create_module(&module);
  }

  if (iree_status_is_ok(status)) {
    status = iree_runtime_session_append_module(session, module);
  }

  // Lookup the entry point function call.
  const char kMainFunctionName[] = "module.simple_mul";
  iree_runtime_call_t call;
  memset(&call, 0, sizeof(call));
  if (iree_status_is_ok(status)) {
    status = iree_runtime_call_initialize_by_name(
        session, iree_make_cstring_view(kMainFunctionName), &call);
  }

  // Populate initial values for 4 * 2 = 8.
  const int kElementCount = 4;
  iree_hal_dim_t shape[1] = {kElementCount};
  iree_hal_buffer_view_t* arg0_buffer_view = NULL;
  iree_hal_buffer_view_t* arg1_buffer_view = NULL;
  float kFloat4[] = {4.0f, 4.0f, 4.0f, 4.0f};
  float kFloat2[] = {2.0f, 2.0f, 2.0f, 2.0f};

  iree_hal_memory_type_t input_memory_type =
      IREE_HAL_MEMORY_TYPE_HOST_LOCAL | IREE_HAL_MEMORY_TYPE_DEVICE_VISIBLE;
  if (iree_status_is_ok(status)) {
    status = iree_hal_buffer_view_clone_heap_buffer(
        iree_hal_device_allocator(device), shape, IREE_ARRAYSIZE(shape),
        IREE_HAL_ELEMENT_TYPE_FLOAT_32, IREE_HAL_ENCODING_TYPE_DENSE_ROW_MAJOR,
        input_memory_type, IREE_HAL_BUFFER_USAGE_ALL,
        iree_make_const_byte_span((void*)kFloat4,
                                  sizeof(float) * kElementCount),
        &arg0_buffer_view);
  }
  if (iree_status_is_ok(status)) {
    status = iree_hal_buffer_view_clone_heap_buffer(
        iree_hal_device_allocator(device), shape, IREE_ARRAYSIZE(shape),
        IREE_HAL_ELEMENT_TYPE_FLOAT_32, IREE_HAL_ENCODING_TYPE_DENSE_ROW_MAJOR,
        input_memory_type, IREE_HAL_BUFFER_USAGE_ALL,
        iree_make_const_byte_span((void*)kFloat2,
                                  sizeof(float) * kElementCount),
        &arg1_buffer_view);
  }

  // Queue buffer views for input.
  if (iree_status_is_ok(status)) {
    status =
        iree_runtime_call_inputs_push_back_buffer_view(&call, arg0_buffer_view);
  }
  iree_hal_buffer_view_release(arg0_buffer_view);

  if (iree_status_is_ok(status)) {
    status =
        iree_runtime_call_inputs_push_back_buffer_view(&call, arg1_buffer_view);
  }
  iree_hal_buffer_view_release(arg1_buffer_view);

  // Invoke call.
  if (iree_status_is_ok(status)) {
    status = iree_runtime_call_invoke(&call, /*flags=*/0);
  }

  // Retreive output buffer view with results from the invocation.
  iree_hal_buffer_view_t* ret_buffer_view = NULL;
  if (iree_status_is_ok(status)) {
    status = iree_runtime_call_outputs_pop_front_buffer_view(&call,
                                                             &ret_buffer_view);
  }

  // Read back the results and ensure we got the right values.
  iree_hal_buffer_mapping_t mapped_memory;
  memset(&mapped_memory, 0, sizeof(mapped_memory));
  if (iree_status_is_ok(status)) {
    status = iree_hal_buffer_map_range(
        iree_hal_buffer_view_buffer(ret_buffer_view),
        IREE_HAL_MEMORY_ACCESS_READ, 0, IREE_WHOLE_BUFFER, &mapped_memory);
  }
  if (iree_status_is_ok(status)) {
    if (mapped_memory.contents.data_length / sizeof(float) != kElementCount) {
      status = iree_make_status(IREE_STATUS_UNKNOWN,
                                "result does not match element count ");
    }
  }
  if (iree_status_is_ok(status)) {
    const float* data = (const float*)mapped_memory.contents.data;
    for (iree_host_size_t i = 0;
         i < mapped_memory.contents.data_length / sizeof(float); ++i) {
      if (data[i] != 8.0f) {
        status = iree_make_status(IREE_STATUS_UNKNOWN, "result mismatches");
      }
    }
  }

  // Cleanup call and buffers.
  iree_hal_buffer_unmap_range(&mapped_memory);
  iree_hal_buffer_view_release(ret_buffer_view);
  iree_runtime_call_deinitialize(&call);

  // Cleanup session and instance.
  iree_hal_device_release(device);
  iree_runtime_session_release(session);
  iree_runtime_instance_release(instance);
  iree_vm_module_release(module);

  return status;
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
static void clock_setup(void) { /* Not implemented */
}

static void gpio_setup(void) { /* Not implemented */
}

static void usart_setup(void) { /* Not implemented */
}

void usart2_send_blocking(uint8_t c) { /* Not implemented */
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

  const iree_status_t result = Run();
  if (!iree_status_is_ok(result)) {
    iree_status_fprint(stderr, result);
    iree_status_free(result);
    return -1;
  } else {
    printf("Execution succesfull!\n");
  }
  print_success();

  while (1) {
  }

  return 0;
}
