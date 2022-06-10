#!/bin/bash

# Copyright 2022 Fraunhofer-Gesellschaft zur Förderung der angewandten Forschung e.V.
#
# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

# Bash script to update (and initalize) the strickly needed IREE submodules.

ORIGINAL_PATH="`pwd`"
PATH_TO_SCRIPT="`dirname $0`"
PATH_TO_IREE="`realpath ${PATH_TO_SCRIPT}/../third_party/iree/`"

if ! [ -f "${PATH_TO_IREE}/CMakeLists.txt" ]; then
  echo "Expected IREE submodule to be initalized: can't find CMakeLists.txt"
  echo "Run \'git submodule update --init\' in your iree-bare-metal-arm clone."
  exit 1
fi

cd ${PATH_TO_IREE}
git submodule update --init -- third_party/googletest
git submodule update --init -- third_party/flatcc
git submodule update --init -- third_party/libyaml
git submodule update --init -- third_party/vulkan_headers
cd ${ORIGINAL_PATH}

echo "Updated the required IREE submodules."
