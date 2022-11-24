# Renode

This folder contains a subset of files imported from [renode/renode](https://github.com/renode/renode/), adjusted to the needs of iree-bare-metal-arm.

- The file [`stm32f407.resc`](https://github.com/renode/renode/blob/4b08e55721d33efaf048c68eca6aabc0846eaf7b/scripts/single-node/stm32f4_discovery.resc) (renamed from `stm32f4_discovery.resc`) was imported at state renode/renode@4b08e55.
- The file [`stm32f407.repl`](https://github.com/renode/renode/blob/381e0c59bd97a1e5f5348222bbeeec5588a68575/platforms/boards/stm32f4_discovery-kit.repl) (renamed from `stm32f4_discovery-kit.repl`) was imported at state renode/renode@381e0c5.
- The file [`stm32f446.repl`](https://github.com/renode/renode/blob/381e0c59bd97a1e5f5348222bbeeec5588a68575/platforms/cpus/stm32f4.repl) (renamed from `stm32f4.repl`) was imported at state renode/renode@381e0c5. It is modified for the STM32F446 according to the [`datasheet`](https://www.st.com/resource/en/datasheet/stm32f446re.pdf) and [`reference manual`](https://www.st.com/resource/en/reference_manual/rm0390-stm32f446xx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf).

Further, we provide files, which use upstream platform description files from [renode/renode](https://github.com/renode/renode/) and modify specific definitions for the needs of the iree-bare-metal-arm project.

- The file `nrf52840.repl` modifies the definition of `uart0` to not use DMA.
- The file `stm32f746.repl` modifies flash and SRAM for the STM32F746 according to the [`datasheet`](https://www.st.com/resource/en/datasheet/stm32f746zg.pdf) and [`reference manual`](https://www.st.com/resource/en/reference_manual/rm0385-stm32f75xxx-and-stm32f74xxx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf).

Moreover, we provide additional files which were specifically created in the context of the iree-bare-metal-arm project.

- The file `stm32l476.repl` defines peripherals for the STM32L476 according to the [`datasheet`](https://www.st.com/resource/en/datasheet/stm32l476rg.pdf) and [`reference manual`](https://www.st.com/resource/en/reference_manual/rm0351-stm32l47xxx-stm32l48xxx-stm32l49xxx-and-stm32l4axxx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf). As [renode/renode](https://github.com/renode/renode/) does not support the RCC for STM32L4 MCUs a minimal Python implementation is provided.
- The file `stm32l4r5.repl` defines peripherals for the STM32L4R5 according to the [`datasheet`](https://www.st.com/resource/en/datasheet/stm32l4r5zi.pdf) and [`reference manual`](https://www.st.com/resource/en/reference_manual/rm0432-stm32l4-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf). As [renode/renode](https://github.com/renode/renode/) does not support the RCC for STM32L4 MCUs a minimal Python implementation is provided.
