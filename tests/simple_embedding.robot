*** Settings ***
Suite Setup                   Setup
Suite Teardown                Teardown
Test Setup                    Reset Emulation
Resource                      ${RENODEKEYWORDS}

*** Test Cases ***
Should Run simple_embedding
    Execute Command         mach create
    Execute Command         machine LoadPlatformDescription @platforms/boards/stm32f4_discovery-kit.repl
    Execute Command         sysbus LoadELF @${ELF}

    Create Terminal Tester  sysbus.uart2

    Start Emulation

    Wait For Line On Uart   Running simple_embedding...
    Wait For Line On Uart   Execution succesfull!
    Wait For Line On Uart   simple_embedding done
