// Copyright 2023 The IREE bare-metal Arm Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <stdio.h>

#include "iree/vm/bytecode/module.h"

#if defined(BUILD_LOADER_HAL)
#include "simple_mul_int_bytecode_module_static_inline_c_c.h"
#else
#include "simple_mul_int_bytecode_module_static_c_c.h"
#endif
// A function to create the bytecode module.
iree_status_t create_module(iree_vm_instance_t* instance,
                            iree_vm_module_t** out_module) {
  const struct iree_file_toc_t* module_file_toc = simple_mul_int_create();
  iree_const_byte_span_t module_data =
      iree_make_const_byte_span(module_file_toc->data, module_file_toc->size);
  return iree_vm_bytecode_module_create(
      instance, module_data, iree_allocator_null(),
      iree_vm_instance_allocator(instance), out_module);
}

void print_success() {
#if defined(BUILD_LOADER_HAL)
  printf("simple_vec_mul_int_bytecode_static_inline passed\n");
#else
  printf("simple_vec_mul_int_bytecode_static passed\n");
#endif
}
