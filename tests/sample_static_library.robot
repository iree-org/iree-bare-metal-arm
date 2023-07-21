# SPDX-FileCopyrightText: 2022 Fraunhofer-Gesellschaft zur Förderung der angewandten Forschung e.V.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

*** Settings ***
Suite Setup         Setup
Suite Teardown      Teardown
Test Setup          Reset Emulation
Resource            ${RENODEKEYWORDS}
Resource            ${TARGET}.resource

*** Variables ***
${EXECUTABLE}       static_library/sample_static_library.elf

*** Test Cases ***
CMSIS
    Run Sample For Library      cmsis           ${EXECUTABLE}
    Output Should Show Success

LIBOPENCM3
    [tags]  libopencm3
    Run Sample For Library      libopencm3      ${EXECUTABLE}
    Output Should Show Success

*** Keywords ***
Output Should Show Success
    Wait For Line On Uart       Execution successful!
    Wait For Line On Uart       static_library_run_bytecode passed
