# Renode

This folder contains a subset of files imported from [renode/renode](https://github.com/renode/renode/), adjusted to our needs.

- The file `nrf52840.repl` imports [nrf52840.repl](https://github.com/renode/renode/blob/master/platforms/cpus/nrf52840.repl) from [renode/renode](https://github.com/renode/renode/) and modifies it to our needs.
- The file [`stm32f407.resc`](https://github.com/renode/renode/blob/4b08e55721d33efaf048c68eca6aabc0846eaf7b/scripts/single-node/stm32f4_discovery.resc) (renamed from `stm32f4_discovery.resc`) was imported at state renode/renode@4b08e55.
- The file [`stm32f407.repl`](https://github.com/renode/renode/blob/381e0c59bd97a1e5f5348222bbeeec5588a68575/platforms/boards/stm32f4_discovery-kit.repl) (renamed from `stm32f4_discovery-kit.repl`) was imported at state renode/renode@381e0c5.
- The file [`stm32f446.repl`](https://github.com/renode/renode/blob/381e0c59bd97a1e5f5348222bbeeec5588a68575/platforms/cpus/stm32f4.repl) (renamed from `stm32f4.repl`) was imported at state renode/renode@381e0c5. It is modified for the STM32F446 according to the [`datasheet`](https://www.st.com/resource/en/datasheet/stm32f446re.pdf) and [`reference manual`](https://www.st.com/resource/en/reference_manual/rm0390-stm32f446xx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf).
- The file `stm32f746.repl` is adapted for the STM32F746 according to the [`datasheet`](https://www.st.com/resource/en/datasheet/stm32f746zg.pdf) and [`reference manual`](https://www.st.com/resource/en/reference_manual/rm0385-stm32f75xxx-and-stm32f74xxx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf).
