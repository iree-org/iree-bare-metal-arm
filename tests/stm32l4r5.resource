*** Settings ***
Documentation       Defining resources for testing the generated samples

*** Variables ***
${MACHINE}          STM32L4R5
${PLATFORMFILE}     @${BASE_DIR}/third_party/renode/stm32l4r5.repl

*** Keywords ***
Run Sample For Library
    [Arguments]             ${lib}  ${executable}

    Setup Machine
    Load Sample             ${executable}   ${lib}
    Run Emulation

Setup Machine
    Execute Command         mach create "${MACHINE}"
    Execute Command         machine LoadPlatformDescription ${PLATFORMFILE}

Load Sample
    [Arguments]             ${executable}   ${lib}

    Execute Command         sysbus LoadELF @${BASE_DIR}/build-${lib}-stm32l4r5/samples/${executable}

Run Emulation
    Create Terminal Tester  sysbus.usart2
    Start Emulation
