# Utils

This folder contains files to configure UART, GPIO and clock for different boards.

## UART
The UART pin configuration is documented in the following tables.

### UART 1

| Mode | Arduino | Morpho | Comment            |
| ---- | ------- | ------ | ------------------ |
| TX   | D8      | PA9    |                    |
| RX   | D2      | PA10   | not configured yet |

### UART 2

| Mode | Arduino | Morpho | Comment                                 |
| ---- | ------- | ------ | --------------------------------------- |
| TX   | D1      | PA2    | not wired on Nucleo                     |
| RX   | D0      | PA3    | not configured yet; not wired on Nucleo |

## Clock

The `PLL` configuration parameters are board specific and defined in the following header files. They can be adjusted to your needs. See the reference manuals for more information. 

* The file `stm32f407xx_cmsis.h` contains `PLL` configuration parameters for the STM32F407 to run at maximum speed of 168MHz.
* The file `stm32f411xe_cmsis.h` contains `PLL` configuration parameters for the STM32F411xE to run at maximum speed of 100MHz.
* The file `stm32f446xx_cmsis.h` contains `PLL` configuration parameters for the STM32F446 to run at maximum speed of 180MHz.
