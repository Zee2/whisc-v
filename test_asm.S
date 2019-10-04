.section .text
.globl _start


_start:
	jal x1, my_func
	addi x6, x5, 5
	beq x0, x0, _start
my_func:
	addi x5, x5, 5
	ret
my_label:
	.word 0xDEADBEEF

