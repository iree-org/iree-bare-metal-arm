#!/bin/bash

# Copyright 2022 Fraunhofer-Gesellschaft zur Förderung der angewandten Forschung e.V.
#
# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

# Bash script to build / install the IREE host tools.

if [[ $# -ne 0 && $# -ne 1 ]] ; then
  echo "Usage: $0"
  echo "   or: $0 -pip"
  echo ""
  echo "By default, the iree-dist tarball is fetched and unpacked."
  echo "In addition, iree-compile can be installed via pip, whereas additional"
  echo "tools are installed from source."
  exit 1
fi


# Check if pip should be used instead of fetching the iree-dist tarball

if [[ $# -eq 1 && "$1" != "-pip" ]]; then
  echo "$0: unrecognized option '${1}'"
  exit 1
fi

if [[ "${VIRTUAL_ENV}" != "" ]]; then
  echo "$0 cannot be executed in a venv since it creates a new venv."
  exit 1;
fi

# Determine which version to install

PATH_TO_SCRIPT="`dirname $0`"
PATH_TO_REPO="`realpath ${PATH_TO_SCRIPT}/../`"

if [[ $# -eq 0 ]] ; then
  IREE_VERSION="`sed -n 's/.*dist-\(.*\)-linux-x86_64.tar.xz/\1/p' ${PATH_TO_REPO}/iree-release-link.txt`"
else
  IREE_VERSION="`sed -n 2p ${PATH_TO_REPO}/requirements-compiler.txt | sed 's/.*=//'`"
fi

echo "Installing IREE host tools candidate-${IREE_VERSION}"


# Create the IREE_HOST_BIN_DIR directory

mkdir -p build-iree-host-install-${IREE_VERSION}/bin
ln -sfn build-iree-host-install-${IREE_VERSION} build-iree-host-install


# Create a virtual environment and install required tool

python3 -m venv venv-iree-${IREE_VERSION}
source venv-iree-${IREE_VERSION}/bin/activate
pip3 install -r ${PATH_TO_REPO}/requirements.txt


# Use the iree-dist tarball

if [[ $# -eq 0 ]] ; then
  wget -i ${PATH_TO_REPO}/iree-release-link.txt -P build-iree-host-install-${IREE_VERSION}
  tar xvfJ build-iree-host-install-${IREE_VERSION}/iree-dist-${IREE_VERSION}-linux-x86_64.tar.xz -C build-iree-host-install-${IREE_VERSION}
  rm build-iree-host-install-${IREE_VERSION}/iree-dist-${IREE_VERSION}-linux-x86_64.tar.xz

  echo ""
  echo -e "Created a virtual environment at '\033[36mvenv-iree-${IREE_VERSION}\033[m'"
  echo -e "\033[32mMake sure to active the virtual environment before building the samples\033[m"
  echo -e "Downloaded and unpacked IREE dist tarball to '\033[36mbuild-iree-host-install-${IREE_VERSION}\033[m'"
  echo -e "Created a symbolic link to '\033[36mbuild-iree-host-install-${IREE_VERSION}\033[m' with the name '\033[36mbuild-iree-host-install\033[m'"
  exit 0
fi


# Use pip + install `generate_embed_data` and `iree-flatcc-cli` from source

pip3 install -r ${PATH_TO_REPO}/requirements-compiler.txt

mkdir -p build-iree-host-tools-${IREE_VERSION}
ln -sfn build-iree-host-tools-${IREE_VERSION} build-iree-host-tools

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
      ${PATH_TO_REPO}/third_party/iree/
cmake --build . --target generate_embed_data iree-flatcc-cli

cp build_tools/embed_data/generate_embed_data ../build-iree-host-install/bin
cp build_tools/third_party/flatcc/iree-flatcc-cli ../build-iree-host-install/bin/
cd ..

echo ""
echo -e "Installed IREE snapshot to '\033[36mvenv-iree-${IREE_VERSION}\033[m'"
echo -e "\033[32mMake sure to active the virtual environment before building the samples\033[m"
echo -e "Copied IREE tools to '\033[36mbuild-iree-host-install-${IREE_VERSION}\033[m'"
echo -e "Created a symbolic link to '\033[36mbuild-iree-host-install-${IREE_VERSION}\033[m' with the name '\033[36mbuild-iree-host-install\033[m'"
