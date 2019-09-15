.section .text
.globl _start
_start:
	add x1, x2, x3
	add x1, x4, x5
	add x1, x6, x7

.section .data
my_label:
	.word 0xDEADBEEF
