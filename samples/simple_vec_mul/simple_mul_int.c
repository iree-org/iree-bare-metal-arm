// Copyright 2021 The IREE Authors
// Copyright 2023 Fraunhofer-Gesellschaft zur Förderung der angewandten Forschung e.V.
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// A example of setting up the HAL module to run simple pointwise array
// multiplication with the device implemented by different backends via
// create_sample_driver().
//
// NOTE: this file does not properly handle error cases and will leak on
// failure. Applications that are just going to exit()/abort() on failure can
// probably get away with the same thing but really should prefer not to.

#include <stdio.h>

#include "iree/base/api.h"
#include "iree/hal/api.h"
#include "iree/modules/hal/inline/module.h"
#include "iree/modules/hal/loader/module.h"
#include "iree/modules/hal/module.h"
#include "iree/vm/api.h"
#include "iree/vm/bytecode/module.h"

// A function to create the HAL device from the different backend targets.
// The HAL device is returned based on the implementation, and it must be
// released by the caller.
extern iree_status_t create_sample_device(
    iree_allocator_t host_allocator, iree_hal_device_t** out_device,
    iree_hal_executable_loader_t** loader);

// A function to create the bytecode or C module.
extern iree_status_t create_module(iree_vm_instance_t* instance,
                                   iree_vm_module_t** out_module);

extern void print_success();

iree_status_t Run() {
  iree_vm_instance_t* instance = NULL;
  IREE_RETURN_IF_ERROR(iree_vm_instance_create(
      IREE_VM_TYPE_CAPACITY_DEFAULT, iree_allocator_system(), &instance));

#if defined(BUILD_INLINE_HAL)
  IREE_RETURN_IF_ERROR(iree_hal_module_register_inline_types(instance));
#elif defined(BUILD_LOADER_HAL)
  IREE_RETURN_IF_ERROR(iree_hal_module_register_loader_types(instance));
#else
  IREE_RETURN_IF_ERROR(iree_hal_module_register_all_types(instance));
#endif

  iree_hal_device_t* device = NULL;
  iree_hal_executable_loader_t* loader = NULL;
  IREE_RETURN_IF_ERROR(
      create_sample_device(iree_allocator_system(), &device, &loader),
      "create device");

  // Create module
  iree_vm_module_t* module = NULL;
  IREE_RETURN_IF_ERROR(create_module(instance, &module));

#if defined(BUILD_INLINE_HAL) || defined(BUILD_LOADER_HAL)
  // Create hal_inline_module
  iree_vm_module_t* hal_inline_module = NULL;
  IREE_RETURN_IF_ERROR(iree_hal_inline_module_create(
      instance, IREE_HAL_INLINE_MODULE_FLAG_NONE,
      iree_hal_device_allocator(device), iree_allocator_system(),
      &hal_inline_module));
#endif

#if defined(BUILD_INLINE_HAL)
  iree_vm_module_t* modules[] = {hal_inline_module, module};
#elif defined(BUILD_LOADER_HAL)
  // Create hal_loader_module
  iree_vm_module_t* hal_loader_module = NULL;
  IREE_RETURN_IF_ERROR(iree_hal_loader_module_create(
      instance, IREE_HAL_MODULE_FLAG_NONE,
      /*loader_count=*/1, &loader, iree_allocator_system(),
      &hal_loader_module));
  iree_vm_module_t* modules[] = {hal_inline_module, hal_loader_module, module};
#else
  // Create hal_module
  iree_vm_module_t* hal_module = NULL;
  IREE_RETURN_IF_ERROR(
      iree_hal_module_create(instance, device, IREE_HAL_MODULE_FLAG_SYNCHRONOUS,
                             iree_allocator_system(), &hal_module));

  iree_vm_module_t* modules[] = {hal_module, module};
#endif

  // Allocate a context that will hold the module state across invocations.
  iree_vm_context_t* context = NULL;

  iree_hal_executable_loader_release(loader);

  IREE_RETURN_IF_ERROR(iree_vm_context_create_with_modules(
      instance, IREE_VM_CONTEXT_FLAG_NONE, IREE_ARRAYSIZE(modules), &modules[0],
      iree_allocator_system(), &context));

#if defined(BUILD_INLINE_HAL) || defined(BUILD_LOADER_HAL)
  iree_vm_module_release(hal_inline_module);
#else
  iree_vm_module_release(hal_module);
#endif

#if defined(BUILD_LOADER_HAL)
  iree_vm_module_release(hal_loader_module);
#endif

  iree_vm_module_release(module);

  // Lookup the entry point function.
  // Note that we use the synchronous variant which operates on pure type/shape
  // erased buffers.
  const char kMainFunctionName[] = "module.simple_mul";
  iree_vm_function_t main_function;
  IREE_RETURN_IF_ERROR(iree_vm_context_resolve_function(
      context, iree_make_cstring_view(kMainFunctionName), &main_function));

  // Initial buffer contents.
  int32_t kInt0[1024];
  int32_t kInt1[1024];
  int32_t results[1024];
  for (int i = 0; i < 1024; ++i) {
    kInt0[i] = i >> 1;
    kInt1[i] = i;
    results[i] = 0;
  }

  // Allocate buffers in device-local memory so that if the device has an
  // independent address space they live on the fast side of the fence.
  iree_hal_dim_t shape[1] = {IREE_ARRAYSIZE(kInt0)};
  iree_hal_buffer_view_t* arg0_buffer_view = NULL;
  iree_hal_buffer_view_t* arg1_buffer_view = NULL;
  IREE_RETURN_IF_ERROR(iree_hal_buffer_view_allocate_buffer_copy(
      device, iree_hal_device_allocator(device), IREE_ARRAYSIZE(shape), shape,
      IREE_HAL_ELEMENT_TYPE_SINT_32, IREE_HAL_ENCODING_TYPE_DENSE_ROW_MAJOR,
      (iree_hal_buffer_params_t){
          .type = IREE_HAL_MEMORY_TYPE_DEVICE_LOCAL,
          .usage = IREE_HAL_BUFFER_USAGE_DEFAULT,
      },
      iree_make_const_byte_span(kInt0, sizeof(kInt0)), &arg0_buffer_view));
  IREE_RETURN_IF_ERROR(iree_hal_buffer_view_allocate_buffer_copy(
      device, iree_hal_device_allocator(device), IREE_ARRAYSIZE(shape), shape,
      IREE_HAL_ELEMENT_TYPE_SINT_32, IREE_HAL_ENCODING_TYPE_DENSE_ROW_MAJOR,
      (iree_hal_buffer_params_t){
          .type = IREE_HAL_MEMORY_TYPE_DEVICE_LOCAL,
          .usage = IREE_HAL_BUFFER_USAGE_DEFAULT,
      },
      iree_make_const_byte_span(kInt1, sizeof(kInt1)), &arg1_buffer_view));

  // Setup call inputs with our buffers.
  iree_vm_list_t* inputs = NULL;
  IREE_RETURN_IF_ERROR(
      iree_vm_list_create(iree_vm_make_undefined_type_def(),
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
  IREE_RETURN_IF_ERROR(
      iree_vm_list_create(iree_vm_make_undefined_type_def(),
                          /*capacity=*/1, iree_allocator_system(), &outputs),
      "can't allocate output vm list");

  // Synchronously invoke the function.
  IREE_RETURN_IF_ERROR(iree_vm_invoke(
      context, main_function, IREE_VM_INVOCATION_FLAG_NONE,
      /*policy=*/NULL, inputs, outputs, iree_allocator_system()));

  // Get the result buffers from the invocation.
  iree_hal_buffer_view_t* ret_buffer_view =
      iree_vm_list_get_buffer_view_assign(outputs, 0);
  if (ret_buffer_view == NULL) {
    return iree_make_status(IREE_STATUS_NOT_FOUND,
                            "can't find return buffer view");
  }

  // Read back the results and ensure we got the right values.
  IREE_RETURN_IF_ERROR(iree_hal_device_transfer_d2h(
      device, iree_hal_buffer_view_buffer(ret_buffer_view), 0, results,
      sizeof(results), IREE_HAL_TRANSFER_BUFFER_FLAG_DEFAULT,
      iree_infinite_timeout()));
  for (iree_host_size_t i = 0; i < IREE_ARRAYSIZE(results); ++i) {
    if (results[i] != (i >> 1) * i) {
      return iree_make_status(IREE_STATUS_UNKNOWN, "result mismatches");
    }
  }

  // Print statistics (no-op if statistics are not enabled).
  /*
  iree_hal_allocator_statistics_fprint(stdout,
                                       iree_hal_device_allocator(device));
  */

  iree_vm_list_release(inputs);
  iree_vm_list_release(outputs);
  iree_hal_device_release(device);
  iree_vm_context_release(context);
  iree_vm_instance_release(instance);
  return iree_ok_status();
}

extern void clock_setup(void);
extern void gpio_setup(void);
extern void usart_setup(void);

int main(void) {
  clock_setup();
  gpio_setup();
  usart_setup();

  printf("Running simple_mul_int...\n");

  const iree_status_t result = Run();
  int ret = (int)iree_status_code(result);
  if (!iree_status_is_ok(result)) {
    iree_status_fprint(stderr, result);
    iree_status_free(result);
  } else {
    printf("Execution successful!\n");
  }
  print_success();

  while (1) {
  }

  return 0;
}
