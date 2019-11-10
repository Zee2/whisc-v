.section .text
.globl _start


_start:
	addi x6, x5, 5
	sb x3, 4(x6)
	blt x4, x6, _target
	nop
_target: 
	nop
	nop
	nop
	nop

