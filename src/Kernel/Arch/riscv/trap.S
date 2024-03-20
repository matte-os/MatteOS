# trap.S
# Trap handler and global context
# Steve Operating System
# Stephen Marz
# 24 February 2019
.option norvc
.altmacro
.set NUM_GP_REGS, 32  # Number of registers per context
.set NUM_FP_REGS, 32
.set REG_SIZE, 8   # Register size (in bytes)
.set MAX_CPUS, 8   # Maximum number of CPUs

# Use macros for saving and restoring multiple registers
.macro save_gp i, basereg=t6
	sd	x\i, ((\i)*REG_SIZE)(\basereg)
.endm
.macro load_gp i, basereg=t6
	ld	x\i, ((\i)*REG_SIZE)(\basereg)
.endm
.macro save_fp i, basereg=t6
	fsd	f\i, ((NUM_GP_REGS+(\i))*REG_SIZE)(\basereg)
.endm
.macro load_fp i, basereg=t6
	fld	f\i, ((NUM_GP_REGS+(\i))*REG_SIZE)(\basereg)
.endm


.section .text.context_switching
.global m_trap_test
m_trap_test:
        ret

.global m_trap_vector
# This must be aligned by 4 since the last two bits
# of the mtvec register do not contribute to the address
# of this vector.
.align 4
m_trap_vector:
	# All registers are volatile here, we need to save them
	# before we do anything.
	csrrw	t6, sscratch, t6
	# csrrw will atomically swap t6 into mscratch and the old
	# m_value of mscratch into t6. This is nice because we just
	# switched values and didn't destroy anything -- all atomically!
	# in cpu.rs we have a structure of:
	#  32 gp regs		0
	#  32 fp regs		256
	#  SATP register	512
	#  Trap stack       520
	#  CPU HARTID		528
	# We use t6 as the temporary register because it is the very
	# bottom register (x31)
	.set 	i, 1
	.rept	30
		save_gp	%i
		.set	i, i+1
	.endr

	# Save the actual t6 register, which we swapped into
	# mscratch
	mv		t5, t6
	csrr	t6, sscratch
	save_gp 31, t5

	# Restore the kernel trap frame into mscratch
	csrw	sscratch, t5

	# Get ready to go into Rust (trap.rs)
	# We don't want to write into the user's stack or whomever
	# messed with us here.
	csrr	a0, sepc
	csrr	a1, stval
	csrr	a2, scause
	li	a3, 0x0
	csrr	a4, sstatus
	mv		a5, t5
	ld		sp, 520(a5)
	call	trap_vector

	# When we get here, we've returned from m_trap, restore registers
	# and return.
	# m_trap will return the return address via a0.

	csrw	sepc, a0

	# Now load the trap frame back into t6
	csrr	t6, sscratch

	# Restore all GP registers
	.set	i, 1
	.rept	31
		load_gp %i
		.set	i, i+1
	.endr

	# Since we ran this loop 31 times starting with i = 1,
	# the last one loaded t6 back to its original m_value.

	sret

.global switch_to_user
switch_to_user:
  # a0 - Frame address
	# a1 - Program counter
  # a2 - SATP
	# Load program counter
	ld		a1, 512(a0)
  ld    a2, 520(a0)
  csrr  a3, satp
  sd    a3, 520(a0)
  csrw  sscratch, a2
  
	# 1 << 7 is MPIE
	# Since user mode is 00, we don't need to set anything
	# in MPP (bits 12:11)
	li		t0, 1 << 5 | 1 << 13 | 1 << 18 
	csrw	sstatus, t0
  la    a1, dummy_process_main
	csrw	sepc, a1
	li		t1, 1 << 1 | 1 << 5 | 1 << 9
	csrw	sie, t1
	la		t2, m_trap_vector
	csrw	stvec, t2
	# A0 is the context frame, so we need to reload it back
	# and mret so we can start running the program.
	mv	t6, a0
	.set	i, 0
	.rept	32
		load_fp %i
		.set i, i+1
	.endr

	.set	i, 1
	.rept	31
		load_gp %i, t6
		.set	i, i+1
	.endr
  csrrw t6, sscratch, t6
  csrw satp, t6
  csrrw t6, sscratch, t6
  sret
