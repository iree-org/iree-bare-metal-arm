# Build Tools

The [`stm32f407xg-libopencm3.ld`](https://github.com/libopencm3/libopencm3-examples/blob/4ddb9db8b911391ccf333f7db3459c03e8b85356/examples/stm32/f4/stm32f4-discovery/stm32f4-discovery.ld) (renamed from stm32f4-discovery.ld) linker script was imported without further modifications from the [libopencm3-examples](https://github.com/libopencm3/libopencm3-examples) repository.
The `stm32f4{xxxx,46xe}-libopencm3.ld` are linker scripts adopted to a other memory configurations.

The `stm32-cmsis-base.ld` and `stm32f407xg-cmsis.ld` linker scripts are based on the the [`gcc_arm.ld`](https://github.com/ARM-software/CMSIS_5/blob/ca1b514243d8e69f1a8190e59de4b0c4ea6bdcaa/Device/_Template_Vendor/Vendor/Device/Source/GCC/gcc_arm.ld) linker script from the [ARM-software/CMSIS_5](https://github.com/ARM-software/CMSIS_5) repository. The `gcc_arm.ld` linker script was split, such that common parts are in `stm32-cmsis-base.ld` whereas the memory layout is defined in `stm32f407xg-cmsis.ld`.
The `stm32f4xxxx-cmsis.ld` is a linker script adopted to a custom memory configuration.
