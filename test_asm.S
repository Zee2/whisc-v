.section .text
.globl _start
_start:
	addi x2, x10, 5 
	la x10, my_label
	sw x5, 0(x10)

.section .data
my_label:
	.word 0xDEADBEEF
