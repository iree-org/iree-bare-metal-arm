# SPDX-FileCopyrightText: 2021 Fraunhofer-Gesellschaft zur Förderung der angewandten Forschung e.V.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

*** Settings ***
Suite Setup         Setup
Suite Teardown      Teardown
Test Setup          Reset Emulation
Resource            ${RENODEKEYWORDS}
Resource            ${TARGET}.resource

*** Variables ***
${NAME}             simple_embedding
${EXECUTABLE}       simple_embedding/sample_embedded_sync.elf

*** Test Cases ***
CMSIS
    [tags]  xfail-nrf52840
    Run Sample For Library      cmsis           ${EXECUTABLE}
    Output Should Show Success

LIBOPENCM3
    [tags]  libopencm3
    Run Sample For Library      libopencm3      ${EXECUTABLE}
    Output Should Show Success

*** Keywords ***
Output Should Show Success
    Wait For Line On Uart       Running ${NAME}...
    Wait For Line On Uart       Execution successful!
    Wait For Line On Uart       ${NAME} done
