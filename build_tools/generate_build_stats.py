# Copyright 2022 Fraunhofer-Gesellschaft zur Förderung der angewandten Forschung e.V.
#
# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""Python script to collect stats of built targets."""

import json
import os

commit_hash = os.environ.get("GITHUB_SHA", None)

libs = ["cmsis", "crt0", "libopencm3"]
targets = {
    "sample_vmvx_sync": os.path.join("samples", "simple_embedding"),
    "sample_embedded_sync": os.path.join("samples", "simple_embedding"),
    "sample_static_library": os.path.join("samples", "static_library"),
    "sample_static_library_c": os.path.join("samples", "static_library"),
}

build_stats = {
    "commit_hash": commit_hash,
    "smt32f4xx": {},
}

for lib in libs:
    build_stats["smt32f4xx"][lib] = {}
    for target, path in targets.items():
        target_path = os.path.join(f"build-{lib}", path, target)

        target_file_size = os.path.getsize(target_path)

        build_stats["smt32f4xx"][lib][target] = {"file_size": target_file_size}

with open("build_stats.json", mode="w", encoding="utf-8") as f:
    json.dump(build_stats, f, indent=2, sort_keys=True)
