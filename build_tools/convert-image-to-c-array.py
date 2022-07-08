#!/usr/bin/env python3
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import argparse

import numpy as np
from PIL import Image


def serialize(input_path: str, data_format: str):
    image = Image.open(input_path)
    if data_format == "uint8":
        data = np.array(image).astype(np.uint8)
        value_literal = lambda e: str(e)
        c_type = "uint8_t"
    else:
        data = np.array(image).astype(np.float32) / 255.0
        value_literal = lambda e: f"{e}f"
        c_type = "float"
    c_array = f"{{{', '.join(map(value_literal, data.flat))}}}"
    print(f"const {c_type} image[] = {c_array};")


def main():
    parser = argparse.ArgumentParser(
        description="Load an image file and serialize it to a C array"
    )
    parser.add_argument("--data-format", choices=["uint8", "float"], default="float")
    parser.add_argument("input", help="Path to input image")
    args = parser.parse_args()

    serialize(args.input, args.data_format)


if __name__ == "__main__":
    main()
