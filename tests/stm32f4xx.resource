*** Settings ***
Documentation       Defining resources for testing the generated samples

*** Variables ***
${MACHINE}          STM32F4XX
${PLATFORMFILE}     @${BASE_DIR}/third_party/renode/stm32f4xx-highmem.repl

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

    Execute Command         sysbus LoadELF @${BASE_DIR}/build-${lib}-stm32f4xx/samples/${executable}

Run Emulation
    Create Terminal Tester  sysbus.usart2
    Start Emulation
