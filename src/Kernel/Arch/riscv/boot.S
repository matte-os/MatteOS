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




#	csrw	sie, zero
#	la		t1, ktest
#	csrw	sepc, t1
#	la		ra, 2f
#	sret

 2:
		
# 	li		t0, (0b01 << 11) | (1 << 7) | (1 << 5)
# 	csrw	sstatus, t0
# 			la		t2, m_trap_vector
# 	csrw	stvec, t2
# 									la		t1, kmain
# 	csrw	sepc, t1
# 							li		t2, 0x888
# 	csrw	sie, t2
# 	mret
# 3:

								
# 		la		sp, _stack_end
# 	li		t0, 0x10000
# 	csrr	a0, mhartid
# 	mul		t0, t0, a0
# 	sub		sp, sp, t0

# 		li		t0, 0b11 << 11 | (1 << 7)
# 	csrw	mstatus, t0
# 			li		t3, (1 << 3)
# 	csrw	mie, t3
# 			la		t1, kinitHart
# 	csrw	mepc, t1
# 					la		t2, m_trap_vector
# 	csrw	mtvec, t2
# 			la		ra, 4f
# 		mret

# 4:
# 				wfi
# 	j		4b

