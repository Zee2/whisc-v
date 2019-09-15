
ASSEMBLER=~/class/ece411/software/riscv-tools/bin/riscv32-unknown-elf-gcc
OBJCOPY=~/class/ece411/software/riscv-tools/bin/riscv32-unknown-elf-objcopy
CFLAGS="-ffreestanding -nostdlib -Wl,--no-relax"

"$ASSEMBLER" $CFLAGS "$@" -o rv32_object.o
"$OBJCOPY" -O binary rv32_object.o test_binary

