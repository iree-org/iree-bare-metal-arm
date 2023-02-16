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


def serialize(input_path: str, data_format: str, resize: str):
    image = Image.open(input_path)
    if not resize == "none":
        w, h = resize.split("x")
        w, h = int(w), int(h)
        image = image.resize((w, h))
    if data_format == "uint8":
        data = np.array(image).astype(np.uint8)
        value_literal = lambda e: str(e)
        c_type = "uint8_t"
    elif data_format in ("float_0_1", "float_1_1"):
        data = np.array(image).astype(np.float32)

        if data_format == "float_0_1":
            low = 0.0
            high = 1.0
        elif data_format == "float_1_1":
            low = -1.0
            high = 1.0
        else:
            raise ValueError("Unsupported data_format")
        
        data = (high - low) * data / 255.0 + low
        value_literal = lambda e: f"{e}f"
        c_type = "float"
    else:
        raise ValueError("Unsupported data_format")

    c_array = f"{{{', '.join(map(value_literal, data.flat))}}}"
    print(f"const {c_type} image[] = {c_array};")


def main():
    parser = argparse.ArgumentParser(
        description="Load an image file and serialize it to a C array"
    )
    parser.add_argument("--data-format", choices=["uint8", "float_0_1", "float_1_1"], default="float_0_1")
    parser.add_argument("--resize", default="none")
    parser.add_argument("input", help="Path to input image")
    args = parser.parse_args()

    serialize(args.input, args.data_format, args.resize)


if __name__ == "__main__":
    main()
