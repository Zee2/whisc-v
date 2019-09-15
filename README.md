# Whisc-V
#### The world's strangest RISC-V dialect for embedded emulators.

Whisc-V is the peculiar RISC-V RTL simulator that powers the Friendly Computer project. Designed to run on resource-constrained embedded systems, Whisc-V is an RTL-level simulator that exposes registers, internal CPU state, and datapath details to the host system, allowing for rich illustration of how the system works.

Ideal for teaching the basics of system architecture, Whisc-V aims to run on the popular STM32 platform, integrating with multiple hardware I/O components for visualizing processor state.


#### Getting Started

For developing/testing on x86 or other desktop platforms, `gcc` is recommended for building the Whisc-V virtual machine itself, and the `riscv-toolchain` flavor of the GNU toolchain is recommended for compiling/assembling test RV32I code. Edit the location variables in the `assemble.sh` script to point to your local install of the riscv-toolchain, and run it with
```
./assemble.sh my_test_asm.S
```
A stripped machine code binary called `test_binary` should be generated. Run the top-level simulator test harness with
```
./whiscv test_binary
```
and observe the results or pipe the simulator output to a log file. Writing your own harness is recommended for embedded use.
