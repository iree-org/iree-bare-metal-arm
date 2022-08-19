*** Settings ***
Documentation       Defining resources for testing the generated samples

*** Variables ***
${MACHINE}          nRF52840
${PLATFORMFILE}     @${BASE_DIR}/third_party/renode/nrf52840.repl

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

    Execute Command         sysbus LoadELF @${BASE_DIR}/build-${lib}-nrf52840/samples/${executable}

Run Emulation
    Create Terminal Tester  sysbus.uart0
    Start Emulation
