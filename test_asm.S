.section .text
.globl _start


_start:
	addi x1, x1, 1
	addi x2, x2, 2
	addi x3, x3, 3
	bez x0, _start

