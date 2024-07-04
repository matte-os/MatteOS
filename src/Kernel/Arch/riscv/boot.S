.option norvc
.section .text.init
.global _start
_start:
.option push
.option norelax
	la		gp, _global_pointer
.option pop
    la		sp, _stack_end
    la 		a2, _bss_start
    la		a3, _bss_end
    bgeu	a2, a3, 2f
    2:
    j kmain
