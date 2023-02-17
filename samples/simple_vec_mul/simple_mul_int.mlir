func.func @simple_mul(%arg0: tensor<1024xi32>, %arg1: tensor<1024xi32>) -> tensor<1024xi32>
{
  %0 = "mhlo.multiply"(%arg0, %arg1) : (tensor<1024xi32>, tensor<1024xi32>) -> tensor<1024xi32>
  return %0 : tensor<1024xi32>
}
