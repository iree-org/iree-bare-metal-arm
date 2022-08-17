# LibOpenCM3

This folder contains a subset of files imported from
[libopencm3/libopencm3](https://github.com/libopencm3/libopencm3) and
[libopencm3/libopencm3-examples](https://github.com/libopencm3/libopencm3-examples)
adjusted to the needs of iree-bare-metal-arm.

- The [`stm32f407xg.ld`](https://github.com/libopencm3/libopencm3-examples/blob/4ddb9db8b911391ccf333f7db3459c03e8b85356/examples/stm32/f4/stm32f4-discovery/stm32f4-discovery.ld) (renamed from stm32f4-discovery.ld) linker script was imported without further modifications from the [libopencm3-examples](https://github.com/libopencm3/libopencm3-examples) repository.

- The `stm32f4{xxxx,46xe}.ld` are linker scripts adopted to other memory configurations.

- The file [`cortex-m-generic.ld`](https://github.com/libopencm3/libopencm3/blob/b88196f8074751cbcef3e3cf7e272bd518aab475/lib/cortex-m-generic.ld) was imported at state libopencm3/libopencm3@b88196f.

- The file [`vector.c`](https://github.com/libopencm3/libopencm3/blob/f53e12d2da3f9730dd842bdf7d1e680cb5877a15/lib/cm3/vector.c) was imported at state libopencm3/libopencm3@f53e12d.
