*** Settings ***
Suite Setup         Setup
Suite Teardown      Teardown
Test Setup          Reset Emulation
Resource            ${RENODEKEYWORDS}
Resource            samples.resource

*** Variables ***
${EXECUTABLE}       sample_static_library

*** Test Cases ***
CMSIS
    Run Sample for Library      cmsis           ${EXECUTABLE}
    Output should show success

LIBOPENCM3
    Run Sample for Library      libopencm3      ${EXECUTABLE}
    Output should show success

*** Keywords ***
Output should show success
    Wait For Line On Uart       Execution succesfull!
    Wait For Line On Uart       static_library_run_bytecode passed
