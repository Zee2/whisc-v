.section .text
.globl _start


_start:
	addi x6, x5, 5
	add x1, x2, x3
	sra x5, x2, x4
	srl x1, x2, x3
	sb x3, 4(x6)
	blt x4, x6, _target
	nop
_target: 
	nop
	nop
	nop
	nop

