*** Settings ***
Suite Setup         Setup
Suite Teardown      Teardown
Test Setup          Reset Emulation
Resource            ${RENODEKEYWORDS}
Resource            samples.resource

*** Variables ***
${MACHINE}          STM23F407
${PLATFORMFILE}     @${BASE_DIR}/third_party/renode/stm32f407.repl
${NAME}             simple_embedding
${EXECUTABLE}       sample_vmvx_sync.elf

*** Test Cases ***
CMSIS
    [tags]  NoCI
    Execute Command         mach create "${MACHINE}"
    Execute Command         machine LoadPlatformDescription ${PLATFORMFILE}
    Execute Command         sysbus LoadELF @${BASE_DIR}/build/samples/${EXECUTABLE}
    Run Emulation
    Output Should Show Success

*** Keywords ***
Output Should Show Success
    Wait For Line On Uart       Running ${NAME}...
    Wait For Line On Uart       Execution successful!
    Wait For Line On Uart       ${NAME} done
