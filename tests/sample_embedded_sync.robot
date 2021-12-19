*** Settings ***
Suite Setup                   Setup
Suite Teardown                Teardown
Test Setup                    Reset Emulation
Resource                      ${RENODEKEYWORDS}

*** Test Cases ***
Should Run sample_embedded_sync
    Execute Command         mach create "STM32F4XX"
    Execute Command         machine LoadPlatformDescription @${BASE_DIR}/third_party/renode/stm32f4xx-highmem.repl
    Execute Command         sysbus LoadELF @${BASE_DIR}/build-cmsis/samples/sample_embedded_sync

    Create Terminal Tester  sysbus.uart2

    Start Emulation

    Wait For Line On Uart   Running simple_embedding...
    Wait For Line On Uart   Execution succesfull!
    Wait For Line On Uart   simple_embedding done
