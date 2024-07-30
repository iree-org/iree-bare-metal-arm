// SPDX-FileCopyrightText: 2022 The IREE bare-metal Arm Authors
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

func.func @simple_mul(%arg0: tensor<4xi32>, %arg1: tensor<4xi32>) -> tensor<4xi32>
    {
  %0 = "arith.muli"(%arg0, %arg1) : (tensor<4xi32>, tensor<4xi32>) -> tensor<4xi32>
  return %0 : tensor<4xi32>
}
