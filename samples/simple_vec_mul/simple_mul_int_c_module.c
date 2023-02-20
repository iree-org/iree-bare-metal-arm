// Copyright 2021 The IREE Authors
// Copyright 2023 Fraunhofer-Gesellschaft zur Förderung der angewandten
//                Forschung e.V.
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include <stdio.h>

#include "simple_mul_int_bytecode_module_static_c_module_emitc.h"

// A function to create the C module.
iree_status_t create_module(iree_vm_instance_t* instance,
                            iree_vm_module_t** out_module) {
  return module_create(instance, iree_vm_instance_allocator(instance),
                       out_module);
}

void print_success() {
  printf("simple_vec_mul_int_bytecode_static_c passed\n");
}
