.section .text
.globl _start
_start:
	addi x1, x0, 420
	addi x1, x0, 69

.section .data
my_label:
	.word 0xDEADBEEF
