// Copyright 2021 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// #define BUILD_WITH_CMSIS 1

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
#include <malloc.h>

// #include "iree/base/api.h"
// #include "iree/hal/api.h"
// #include "iree/modules/hal/module.h"
// #include "iree/vm/api.h"
// #include "iree/vm/bytecode_module.h"

#include "iree/base/api.h"
#include "iree/base/target_platform.h"
#include "iree/hal/local/elf/elf_module.h"
#include "iree/hal/local/executable_library.h"

// ELF modules for various platforms embedded in the binary:
#include "iree/hal/local/elf/testdata/simple_mul_dispatch.h"

static iree_status_t query_arch_test_file_data(
    iree_const_byte_span_t* out_file_data) {
  *out_file_data = iree_make_const_byte_span(NULL, 0);

  iree_string_view_t pattern = iree_string_view_empty();
#if defined(IREE_ARCH_ARM_32)
  pattern = iree_make_cstring_view("*_arm_32.so");
#else
#warning "No architecture pattern specified; ELF linker will not be tested"
#endif  // IREE_ARCH_*

  if (!iree_string_view_is_empty(pattern)) {
    for (size_t i = 0; i < simple_mul_dispatch_size(); ++i) {
      const struct iree_file_toc_t* file_toc = &simple_mul_dispatch_create()[i];
      if (iree_string_view_match_pattern(iree_make_cstring_view(file_toc->name),
                                         pattern)) {
        *out_file_data =
            iree_make_const_byte_span(file_toc->data, file_toc->size);
        return iree_ok_status();
      }
    }
  }

  return iree_make_status(IREE_STATUS_NOT_FOUND,
                          "no architecture-specific ELF binary embedded into "
                          "the application for the current target platform");
}

static iree_status_t run_test() {
  iree_const_byte_span_t file_data;
  IREE_RETURN_IF_ERROR(query_arch_test_file_data(&file_data));

  iree_elf_import_table_t import_table;
  memset(&import_table, 0, sizeof(import_table));
  iree_elf_module_t module;
  IREE_RETURN_IF_ERROR(iree_elf_module_initialize_from_memory(
      file_data, &import_table, iree_allocator_system(), &module));

  void* query_fn_ptr = NULL;
  IREE_RETURN_IF_ERROR(iree_elf_module_lookup_export(
      &module, IREE_HAL_EXECUTABLE_LIBRARY_EXPORT_NAME, &query_fn_ptr));

  union {
    const iree_hal_executable_library_header_t** header;
    const iree_hal_executable_library_v0_t* v0;
  } library;
  library.header =
      (const iree_hal_executable_library_header_t**)iree_elf_call_p_ip(
          query_fn_ptr, IREE_HAL_EXECUTABLE_LIBRARY_LATEST_VERSION,
          /*reserved=*/NULL);
  if (library.header == NULL) {
    return iree_make_status(IREE_STATUS_NOT_FOUND, "library header is empty");
  }

  const iree_hal_executable_library_header_t* header = *library.header;
  if (header->version != IREE_HAL_EXECUTABLE_LIBRARY_VERSION_0) {
    return iree_make_status(IREE_STATUS_INVALID_ARGUMENT,
                            "library version error");
  }

  if (strncmp(header->name, "simple_mul_dispatch_0", strlen(header->name)) !=
      0) {
    return iree_make_status(IREE_STATUS_INVALID_ARGUMENT,
                            "library name mismatches");
  }

  if (library.v0->exports.count != 1) {
    return iree_make_status(IREE_STATUS_INVALID_ARGUMENT,
                            "entry point count mismatches");
  }

  // ret0 = arg0 * arg1
  float arg0[4] = {1.0f, 2.0f, 3.0f, 4.0f};
  float arg1[4] = {100.0f, 200.0f, 300.0f, 400.0f};
  float ret0[4] = {0.0f, 0.0f, 0.0f, 0.0f};
  const float expected[4] = {100.0f, 400.0f, 900.0f, 1600.0f};

  size_t binding_lengths[3] = {
      sizeof(arg0),
      sizeof(arg1),
      sizeof(ret0),
  };
  void* binding_ptrs[3] = {
      arg0,
      arg1,
      ret0,
  };
  iree_hal_vec3_t workgroup_count = {{1, 1, 1}};
  iree_hal_vec3_t workgroup_size = {{1, 1, 1}};
  iree_hal_executable_dispatch_state_v0_t dispatch_state;
  memset(&dispatch_state, 0, sizeof(dispatch_state));
  dispatch_state.workgroup_count = workgroup_count;
  dispatch_state.workgroup_size = workgroup_size;
  dispatch_state.binding_count = 1;
  dispatch_state.binding_lengths = binding_lengths;
  dispatch_state.binding_ptrs = binding_ptrs;
  iree_hal_vec3_t workgroup_id = {{0, 0, 0}};
  void* local_memory = NULL;
  int ret = iree_elf_call_i_ppp((const void*)library.v0->exports.ptrs[0],
                                (void*)&dispatch_state, (void*)&workgroup_id,
                                local_memory);
  if (ret != 0) {
    return iree_make_status(IREE_STATUS_INTERNAL,
                            "dispatch function returned failure: %d", ret);
  }

  iree_status_t status = iree_ok_status();
  for (int i = 0; i < IREE_ARRAYSIZE(expected); ++i) {
    if (ret0[i] != expected[i]) {
      status =
          iree_make_status(IREE_STATUS_INTERNAL,
                           "output mismatch: ret[%d] = %.1f, expected %.1f", i,
                           ret0[i], expected[i]);
      break;
    }
  }

  iree_elf_module_deinitialize(&module);
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
#ifdef BUILD_WITH_CMSIS
        usart2_send_blocking('\r');
#endif
#ifdef BUILD_WITH_LIBOPENCM3
        usart_send_blocking(USART2, '\r');
#endif
      }
#ifdef BUILD_WITH_CMSIS
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

  printf("Running elf module test...\n");

  struct mallinfo mi;

  size_t megabyte = 1024 * 1024;
  size_t size = 127;

  mi = mallinfo(); // printf ("%d\n", mi.uordblks);
  size_t start = mi.uordblks;

  void* a = calloc(size * megabyte, 1);
  mi = mallinfo(); printf ("%d\n", (mi.uordblks-start)/megabyte);
  start = mi.uordblks;
  
  void* b = calloc(size * megabyte, 1);
  mi = mallinfo(); printf ("%d\n", (mi.uordblks-start)/megabyte);

  memset(a, (unsigned int)0, size * megabyte);
  memset(b, (unsigned int)1, size * megabyte);

  printf("A %p <> B %p\n", a, b);
  printf("A %d %d %d <> B %d %d %d\n", ((unsigned int*)a)[0],
         ((unsigned int*)a)[1], ((unsigned int*)a)[2], ((unsigned int*)b)[0],
         ((unsigned int*)b)[1], ((unsigned int*)b)[2]);

  memcpy(b, a, size * megabyte);

  printf("A %p <> B %p\n", a, b);
  printf("A %d %d %d <> B %d %d %d\n", ((unsigned int*)a)[0],
         ((unsigned int*)a)[1], ((unsigned int*)a)[2], ((unsigned int*)b)[0],
         ((unsigned int*)b)[1], ((unsigned int*)b)[2]);
  printf("memcmp result %d\n", memcmp(a, b, sizeof(a)));

  const iree_status_t result = run_test();
  int ret = (int)iree_status_code(result);
  if (!iree_status_is_ok(result)) {
    iree_status_fprint(stderr, result);
    iree_status_free(result);
  } else {
    printf("Execution successful!\n");
  }
  printf("elf module test done\n");

  while (1) {
  }

  return 0;
}
