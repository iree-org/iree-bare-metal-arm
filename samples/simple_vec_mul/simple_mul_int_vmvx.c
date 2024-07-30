// Copyright 2023 The IREE bare-metal Arm Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <stdio.h>

#include "iree/vm/bytecode/module.h"

#if defined(BUILD_INLINE_HAL)
#include "simple_mul_int_bytecode_module_vmvx_inline_c.h"
#else
#include "simple_mul_int_bytecode_module_vmvx_c.h"
#endif

// A function to create a VMVX bytecode module.
iree_status_t create_module(iree_vm_instance_t* instance,
                            iree_vm_module_t** out_module) {
  const struct iree_file_toc_t* module_file_toc =
      simple_mul_int_bytecode_module_vmvx_create();
  iree_const_byte_span_t module_data =
      iree_make_const_byte_span(module_file_toc->data, module_file_toc->size);
  return iree_vm_bytecode_module_create(instance, module_data,
                                        iree_allocator_null(),
                                        iree_allocator_system(), out_module);
}

void print_success() {
#if defined(BUILD_INLINE_HAL)
  printf("simple_vec_mul_int_bytecode_vmvx_inline passed\n");
#else
  printf("simple_vec_mul_int_bytecode_vmvx passed\n");
#endif
}
