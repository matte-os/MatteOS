##
## @file trap.S
## @author Matěj Bucek
##
## Some parts of this file were taken and inspired from OSBlog
.option norvc
.section .text.init
.global _start
_start:
.option push
.option norelax
	la		gp, _global_pointer
.option pop
    la		sp, _stack_end
    j kmain

