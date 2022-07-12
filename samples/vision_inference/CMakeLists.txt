#-------------------------------------------------------------------------------
# Vision inference sample, EmitC
#-------------------------------------------------------------------------------

add_executable(mnist_static_library_c "")

set(_COMPILE_ARGS)
list(APPEND _COMPILE_ARGS "--iree-input-type=mhlo")
list(APPEND _COMPILE_ARGS "--iree-mlir-to-vm-c-module")
list(APPEND _COMPILE_ARGS "--iree-hal-target-backends=dylib-llvm-aot")
list(APPEND _COMPILE_ARGS "--iree-llvm-target-triple=${IREE_LLVM_TARGET_TRIPLE}")
list(APPEND _COMPILE_ARGS "--iree-llvm-target-cpu=${ARM_CPU}")
list(APPEND _COMPILE_ARGS "--iree-llvm-target-float-abi=${IREE_LLVM_TARGET_FLOAT_ABI}")
list(APPEND _COMPILE_ARGS "--iree-llvm-link-embedded=false")
list(APPEND _COMPILE_ARGS "--iree-llvm-link-static")
list(APPEND _COMPILE_ARGS "--iree-llvm-static-library-output-path=mnist_c_module.o")
list(APPEND _COMPILE_ARGS "${IREE_SOURCE_DIR}/samples/models/mnist.mlir")
list(APPEND _COMPILE_ARGS "-o")
list(APPEND _COMPILE_ARGS "mnist_emitc.h")

add_custom_command(
  OUTPUT
    ${CMAKE_CURRENT_BINARY_DIR}/mnist_c_module.h
    ${CMAKE_CURRENT_BINARY_DIR}/mnist_c_module.o
    ${CMAKE_CURRENT_BINARY_DIR}/mnist_emitc.h
  COMMAND ${_COMPILE_TOOL_EXECUTABLE} ${_COMPILE_ARGS}
)

add_library(mnist_c_module
  STATIC
    ${CMAKE_CURRENT_BINARY_DIR}/mnist_c_module.o)

target_sources(mnist_c_module
  PRIVATE
    mnist_c_module.h
    mnist_emitc.h
)

target_compile_definitions(mnist_c_module
  PUBLIC
    EMITC_IMPLEMENTATION
)

set_target_properties(mnist_c_module
  PROPERTIES
  LINKER_LANGUAGE C
)

set(_CONVERT_INPUT_SCRIPT "${CMAKE_SOURCE_DIR}/build_tools/convert-image-to-c-array.py")
set(_MODEL_INPUT "${IREE_SOURCE_DIR}/samples/vision_inference/mnist_test.png")
add_custom_command(
  OUTPUT
    ${CMAKE_CURRENT_BINARY_DIR}/model_input.h
  COMMAND
    ${_CONVERT_INPUT_SCRIPT} ${_MODEL_INPUT} > ${CMAKE_CURRENT_BINARY_DIR}/model_input.h
  DEPENDS ${_CONVERT_INPUT_SCRIPT} ${_MODEL_INPUT}
)

add_library(mnist_model_input
  INTERFACE
    ${CMAKE_CURRENT_BINARY_DIR}/model_input.h
)

target_sources(mnist_static_library_c
  PRIVATE
    static_library_mnist.c
    create_c_module.c
)

target_include_directories(mnist_static_library_c
  PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(mnist_static_library_c
  PRIVATE
    mnist_c_module
    mnist_model_input
    iree::runtime
    iree::hal::drivers::local_sync::sync_driver
    iree::hal::local::loaders::static_library_loader
    iree::vm::shims_emitc
    firmware
    utils
)

add_binary(mnist_static_library_c)
add_ihex(mnist_static_library_c)
