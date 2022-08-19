*** Settings ***
Suite Setup         Setup
Suite Teardown      Teardown
Test Setup          Reset Emulation
Resource            ${RENODEKEYWORDS}
Resource            ${TARGET}.resource

*** Variables ***
${EXECUTABLE}       vision_inference/mnist_static_library.elf

*** Test Cases ***
CMSIS
    Run Sample For Library      cmsis           ${EXECUTABLE}
    Output Should Show Success

LIBOPENCM3
    Run Sample For Library      libopencm3      ${EXECUTABLE}
    Output Should Show Success

*** Keywords ***
Output Should Show Success
    Wait For Line On Uart       Result[0] = 0.000080
    Wait For Line On Uart       Result[1] = 0.004654
    Wait For Line On Uart       Result[2] = 0.000726
    Wait For Line On Uart       Result[3] = 0.000487
    Wait For Line On Uart       Result[4] = 0.958271
    Wait For Line On Uart       Result[5] = 0.002669
    Wait For Line On Uart       Result[6] = 0.011364
    Wait For Line On Uart       Result[7] = 0.001507
    Wait For Line On Uart       Result[8] = 0.008866
    Wait For Line On Uart       Result[9] = 0.011377
    Wait For Line On Uart       Execution successful!
    Wait For Line On Uart       mnist_static_library_run passed
