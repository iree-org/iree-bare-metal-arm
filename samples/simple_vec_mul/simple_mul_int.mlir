// SPDX-FileCopyrightText: 2023 The IREE bare-metal Arm Authors
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

func.func @simple_mul(%arg0: tensor<1024xi32>, %arg1: tensor<1024xi32>) -> tensor<1024xi32>
{
  %0 = "stablehlo.multiply"(%arg0, %arg1) : (tensor<1024xi32>, tensor<1024xi32>) -> tensor<1024xi32>
  return %0 : tensor<1024xi32>
}
