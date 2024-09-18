// Copyright 2021 The IREE Authors
// Copyright 2022 The IREE bare-metal Arm Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// An example to demonstrate how to run a MNIST handwritten digit detection
// vision model with IREE on bare-metal Arm.
//
// The demo is based on the `sample_static_library_c` demo and our/IREE's
// `static_library_demo.c`, respectively.
//
// Note: this demo requires artifacts from iree-compile before it will run.

#include <stdio.h>

#include "iree/hal/drivers/local_sync/sync_device.h"
#include "iree/hal/local/loaders/static_library_loader.h"
#include "iree/modules/hal/module.h"
#include "iree/runtime/api.h"

// Converted image embedded here:
#include "model_input.h"

extern const iree_hal_executable_library_header_t**
mnist_linked_llvm_cpu_library_query(
    iree_hal_executable_library_version_t max_version,
    const iree_hal_executable_environment_v0_t* environment);
// A function to create the bytecode or C module.
extern iree_status_t create_module(iree_vm_instance_t* instance,
                                   iree_vm_module_t** out_module);

extern void print_success();

// A function to create the HAL device from the different backend targets.
// The HAL device is returned based on the implementation, and it must be
// released by the caller.
iree_status_t create_device_with_static_loader(iree_allocator_t host_allocator,
                                               iree_hal_device_t** out_device) {
  // Set paramters for the device created in the next step.
  iree_hal_sync_device_params_t params;
  iree_hal_sync_device_params_initialize(&params);

  // Register the statically linked executable library.
  const iree_hal_executable_library_query_fn_t libraries[] = {
      mnist_linked_llvm_cpu_library_query,
  };
  iree_hal_executable_loader_t* library_loader = NULL;
  iree_status_t status = iree_hal_static_library_loader_create(
      IREE_ARRAYSIZE(libraries), libraries,
      iree_hal_executable_import_provider_null(), host_allocator,
      &library_loader);

  // Use the default host allocator for buffer allocations.
  iree_string_view_t identifier = iree_make_cstring_view("local-sync");
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
  iree_runtime_instance_options_initialize(&instance_options);
  iree_runtime_instance_options_use_all_available_drivers(&instance_options);
  iree_runtime_instance_t* instance = NULL;

  if (iree_status_is_ok(status)) {
    status = iree_runtime_instance_create(&instance_options,
                                          iree_allocator_system(), &instance);
  }

  // Create local device with static loader.
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
    status =
        create_module(iree_runtime_instance_vm_instance(instance), &module);
  }

  if (iree_status_is_ok(status)) {
    status = iree_runtime_session_append_module(session, module);
  }

  // Lookup the entry point function call.
  const char kMainFunctionName[] = "module.predict";
  iree_runtime_call_t call;
  memset(&call, 0, sizeof(call));
  if (iree_status_is_ok(status)) {
    status = iree_runtime_call_initialize_by_name(
        session, iree_make_cstring_view(kMainFunctionName), &call);
  }

  // Initial buffer contents
  const int kElementCount = 784;
  iree_hal_dim_t shape[4] = {1, 28, 28, 1};

  iree_hal_buffer_view_t* arg0_buffer_view = NULL;

  if (iree_status_is_ok(status)) {
    status = iree_hal_buffer_view_allocate_buffer_copy(
        device, iree_hal_device_allocator(device), IREE_ARRAYSIZE(shape), shape,
        IREE_HAL_ELEMENT_TYPE_FLOAT_32, IREE_HAL_ENCODING_TYPE_DENSE_ROW_MAJOR,
        (iree_hal_buffer_params_t){
            .type = IREE_HAL_MEMORY_TYPE_DEVICE_LOCAL,
            .usage = IREE_HAL_BUFFER_USAGE_DEFAULT,
        },
        iree_make_const_byte_span(image, sizeof(float) * kElementCount),
        &arg0_buffer_view);
  }

  // Queue buffer views for input.
  if (iree_status_is_ok(status)) {
    status =
        iree_runtime_call_inputs_push_back_buffer_view(&call, arg0_buffer_view);
  }
  iree_hal_buffer_view_release(arg0_buffer_view);

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

  // Read back the results
  float results[] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                     0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
  if (iree_status_is_ok(status)) {
    status = iree_hal_device_transfer_d2h(
        device, iree_hal_buffer_view_buffer(ret_buffer_view), 0, results,
        sizeof(results), IREE_HAL_TRANSFER_BUFFER_FLAG_DEFAULT,
        iree_infinite_timeout());
  }
  if (iree_status_is_ok(status)) {
    for (iree_host_size_t i = 0; i < IREE_ARRAYSIZE(results); ++i) {
      printf("Result[%d] = %f\n", i, results[i]);
    }
  }

  // Print statistics (no-op if statistics are not enabled).
  iree_hal_allocator_statistics_fprint(stdout,
                                       iree_hal_device_allocator(device));

  // Cleanup call and buffers.
  iree_hal_buffer_view_release(ret_buffer_view);
  iree_runtime_call_deinitialize(&call);

  // Cleanup session and instance.
  iree_hal_device_release(device);
  iree_runtime_session_release(session);
  iree_runtime_instance_release(instance);
  iree_vm_module_release(module);

  return status;
}

extern void clock_setup(void);
extern void gpio_setup(void);
extern void usart_setup(void);

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
    printf("Execution successful!\n");
  }
  print_success();

  while (1) {
  }

  return 0;
}
