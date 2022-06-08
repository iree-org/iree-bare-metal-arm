#!/bin/bash

# Copyright 2022 Fraunhofer-Gesellschaft zur Förderung der angewandten Forschung e.V.
#
# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

# Bash script to build / install the IREE host tools.


PATH_TO_SCRIPT="`dirname $0`"
PATH_TO_REQUIREMENTS="`realpath ${PATH_TO_SCRIPT}/../requirements.txt`"
IREE_VERSION="`sed -n 2p ${PATH_TO_REQUIREMENTS} | sed 's/.*=//'`"

echo ${IREE_VERSION}

python3 -m venv venv-iree-snapshot-${IREE_VERSION}
source venv-iree-snapshot-${IREE_VERSION}/bin/activate
pip3 install -r requirements.txt

mkdir build-iree-host-tools-${IREE_VERSION}
ln -sfn build-iree-host-tools-${IREE_VERSION} build-iree-host-tools

mkdir -p build-iree-host-install-${IREE_VERSION}/bin
ln -sfn build-iree-host-install-${IREE_VERSION} build-iree-host-install

cd build-iree-host-tools

cmake -GNinja \
      -DCMAKE_C_COMPILER=clang \
      -DCMAKE_CXX_COMPILER=clang++ \
      -DIREE_ERROR_ON_MISSING_SUBMODULES=OFF \
      -DIREE_ENABLE_THREADING=OFF \
      -DIREE_HAL_DRIVER_DEFAULTS=OFF \
      -DIREE_BUILD_COMPILER=OFF \
      -DIREE_BUILD_SAMPLES=OFF \
      -DIREE_BUILD_TESTS=OFF \
      -DCMAKE_INSTALL_PREFIX=../build-iree-host-install \
      ../third_party/iree/
cmake --build . --target generate_embed_data iree-flatcc-cli

cp build_tools/embed_data/generate_embed_data ../build-iree-host-install/bin
cp build_tools/third_party/flatcc/iree-flatcc-cli ../build-iree-host-install/bin/
cd ..
