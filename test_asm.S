.section .text
.globl _start


_start:
	lw x1, my_label
my_label:
	.word 0xDEADBEEF

