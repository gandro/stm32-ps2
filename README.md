stm32-ps2
=========

This project aims to write an PS/2 keyboard device emulator on the [STM32 Discovery](http://www.st.com/internet/evalboard/product/250863.jsp), an evaluation board with the STM32F100RB microcontroller (128 KB Flash, 8 KB RAM).

Prerequisites
-------------
 - GCC based ARM Toolchain, i.e. [Sourcery CodeBench Lite Edition](http://www.mentor.com/embedded-software/sourcery-tools/sourcery-codebench/editions/lite-edition/) or [summon-arm-toolchain](https://github.com/esden/summon-arm-toolchain)
 - STM32 Discovery programmer tool, i.e. [texane/stlink](https://github.com/texane/stlink)

Usage
-----

 - To compile the project and generate the corresponding `.bin` firmware, run `make`.
 - Run `make flash` to write the generated `.bin` file to the STM32 flash memory.
 - `make gdb-server` and `make gdb-client` can be used to invoke the debugger service and client.

Libraries
---------

This repository contains code from the [STM32F10x Standard Peripherals Library v3.5.0](http://www.st.com/web/catalog/tools/FM147/CL1794/SC961/SS1743/PF257890) and the [tinyprintf](https://github.com/cjlano/tinyprintf) library.
