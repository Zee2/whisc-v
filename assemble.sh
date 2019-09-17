
ASSEMBLER=~/class/ece411/software/riscv-tools/bin/riscv32-unknown-elf-gcc
OBJCOPY=~/class/ece411/software/riscv-tools/bin/riscv32-unknown-elf-objcopy
OBJDUMP=~/class/ece411/software/riscv-tools/bin/riscv32-unknown-elf-objdump
CFLAGS="-ffreestanding -nostdlib -Wl,--no-relax"

"$ASSEMBLER" $CFLAGS "$@" -o rv32_object.o
"$OBJCOPY" -O binary rv32_object.o test_binary
"$OBJDUMP" -D rv32_object.o -Mnumeric -Mno-aliases
