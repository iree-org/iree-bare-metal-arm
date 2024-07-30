// Copyright 2022 The IREE bare-metal Arm Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <errno.h>
#include <unistd.h>

extern void send_blocking(uint16_t c);

int _write(int file, char* ptr, int len) {
  int i;

  if (file == STDOUT_FILENO || file == STDERR_FILENO) {
    for (i = 0; i < len; i++) {
      if (ptr[i] == '\n') {
        send_blocking('\r');
      }
      send_blocking(ptr[i]);
    }
    return i;
  }
  errno = EIO;
  return -1;
}
