##
## @file trap.S
## @author Matěj Bucek
##
## Some parts of this file were taken and inspired from OSBlog
.option norvc
.altmacro
.set NUM_GP_REGS, 32  # Number of registers per context
.set NUM_FP_REGS, 32
.set REG_SIZE, 8   # Register size (in bytes)
.set MAX_CPUS, 8   # Maximum number of CPUs

# Use macros for saving and restoring multiple registers
# These were taken from the OS Blog by Stephen Marz
# Copyright (c) 2019 Stephen Marz
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
# of the stvec register do not contribute to the address
# of this vector.
.align 4
m_trap_vector:
	# All registers are volatile here, we need to save them
	# before we do anything.
	csrrw	t6, sscratch, t6
	# Put the trap frame into t6
	li t6, 0x1000
    # Save all GP registers
    # This also svaesthe SP, GP etc. as their are a part of the GP registers
	.set 	i, 0
	.rept	31
		save_gp	%i
		.set	i, i+1
	.endr

	# TODO: Save all FP registers
    .set 	i, 0
    .rept	32
        save_fp	%i
        .set	i, i+1
    .endr

	# Save the actual t6 register, which we swapped into
	# sscratch
	csrr a0, sscratch
	mv t5, t6
	mv t6, a0
	save_gp 31, t5
	mv t6, t5

    # Save the userland SATP
    csrr t5, satp
    csrw sscratch, t5

    # Load the kernel SATP
    ld t4, 520(t6)
    # Store the userland SATP to TrapFrame
    sd t5, 520(t6)

    # Switch memory to Kernel Mode
    csrw satp, t4

    # If kernel SATP(t4) & userland SATP(t5)
    # are the same, trap occurred in the
    # kernel itself.
    beq t4, t5, set_kernel_flag
    li a5, 0
    j skip_kernel_flag

    set_kernel_flag:
    li a5, 1

    skip_kernel_flag:

    # Load the kernel trap frame
    li t6, 0x1000
    # Load the stack pointer
    ld sp, 16(t6)
    # Load the kernel cpu_id
    ld a3, 536(t6)

    # Now we can work in th kernelsapce:
	csrr	a0, sepc
	csrr	a1, stval
	csrr	a2, scause
	csrr	a4, sstatus
	call	handle_interrupt

	# trap_vector will return the return address via a0.
	csrw	sepc, a0

	# Restore the userland SATP
    csrr t5, sscratch
    csrr t4, satp
    csrw satp, t5

    li t6, 0x1000
    # Store kernel SATP
    sd t4, 520(t6)

	# Restore all GP registers
	.set	i, 1
	.rept	31
		load_gp %i
		.set	i, i+1
	.endr

	sret

.global switch_to_user
switch_to_user:
    # a0: Trap frame from C++
    # a1: Program counter
    # a2: Userland SATP
    # Load program counter
    ld	  a1, 512(a0)
    ld    a2, 520(a0)

    # Save the kernel SATP to the process trap frame
    csrr  a3, satp
    sd    a3, 520(a0)

    # Set correct bits to sstatus
    # 1 << 5 is SPIE
    # 1 << 13 is FS
    # 1 << 18 is SUM
    li		t0, 1 << 5 | 1 << 13 | 1 << 18
    csrw	sstatus, t0

    # Set sepc to the process' program counter
    csrw	sepc, a1

    # Enable individual interrupts
    # 1 << 1 is Supervisor Interrupt Enable
    # 1 << 5 is Supervisor Timer Interrupt Enable
    # 1 << 9 is Supervisor External Interrupt Enable
    li		t1, 1 << 1 | 1 << 5 | 1 << 9
    csrw	sie, t1

    # Set the trap vector to m_trap_vector
    la		t2, m_trap_vector
    csrw	stvec, t2

    # Switch to userland address space
    csrw satp, a2

    # Load the trap frame userland address
    li t6, 0x1000

    # Load the process' t6 original value
    ld t5, 256(t6)
    csrw sscratch, t5

    # Load the process' fp registers
    .set	i, 0
    .rept	32
        load_fp %i
        .set i, i+1
    .endr

    # Load the process' gp registers
    .set	i, 1
    .rept	31
        load_gp %i, t6
        .set	i, i+1
    .endr

    # Restore the t6 original value
    csrrw t6, sscratch, t6

    # Context switch to userland
    sret
