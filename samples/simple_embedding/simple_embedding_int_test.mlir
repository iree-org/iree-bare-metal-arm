func @simple_mul(%arg0: tensor<4xi32>, %arg1: tensor<4xi32>) -> tensor<4xi32>
    {
  %0 = "mhlo.multiply"(%arg0, %arg1) {name = "mul.1"} : (tensor<4xi32>, tensor<4xi32>) -> tensor<4xi32>
  return %0 : tensor<4xi32>
}
