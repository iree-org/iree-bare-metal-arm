*** Settings ***
Suite Setup         Setup
Suite Teardown      Teardown
Test Setup          Reset Emulation
Resource            ${RENODEKEYWORDS}
Resource            samples.resource

*** Variables ***
${NAME}             simple_embedding
${EXECUTABLE}       sample_embedded_sync

*** Test Cases ***
CMSIS
    Run Sample For Library      cmsis           ${EXECUTABLE}
    Output Should Show Success

CRT0
    Run Sample For Library      crt0            ${EXECUTABLE}
    Output Should Show Success

LIBOPENCM3
    Run Sample For Library      libopencm3      ${EXECUTABLE}
    Output Should Show Success

*** Keywords ***
Output Should Show Success
    Wait For Line On Uart       Running ${NAME}...
    Wait For Line On Uart       Execution succesfull!
    Wait For Line On Uart       ${NAME} done
