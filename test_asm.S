.section .text
.globl _start


_start:
	lw x1, my_label
	add x2, x2, x1
	beq x0, x0, _start
my_label:
	.word 0xDEADBEEF

