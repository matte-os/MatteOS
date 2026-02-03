.option norvc
.altmacro

# =============================================================
# CONSTANTS & CONFIGURATION
# =============================================================
.set REG_SIZE, 8
.set TRAP_FRAME_ALIAS, 0x1000

# =============================================================
# STRUCT OFFSETS (Matches your C++ TrapFrame)
# =============================================================
.set OFF_REGS,      0
.set OFF_FREGS,     256
.set OFF_SEPC,      512
.set OFF_SSTATUS,   520
.set OFF_SCAUSE,    528
.set OFF_STVAL,     536
.set OFF_DUAL_SATP, 544  # <--- Dual Use: Holds User SATP when in Kernel, Kernel SATP when in User
.set OFF_K_SP,      552
.set OFF_K_TP,      560
.set OFF_K_ADDR,    568

# =============================================================
# MACROS
# =============================================================
.macro save_gp i, basereg=t6
    sd x\i, ((\i)*REG_SIZE + OFF_REGS)(\basereg)
.endm

.macro load_gp i, basereg=t6
    ld x\i, ((\i)*REG_SIZE + OFF_REGS)(\basereg)
.endm

.macro save_fp i, basereg=t6
    fsd f\i, ((\i)*REG_SIZE + OFF_FREGS)(\basereg)
.endm

.macro load_fp i, basereg=t6
    fld f\i, ((\i)*REG_SIZE + OFF_FREGS)(\basereg)
.endm

.section .text.context_switching

# =============================================================
# TRAP VECTOR (Entry Point)
# =============================================================
.global m_trap_vector
.align 4
m_trap_vector:
    # 1. THE SWAP (t6 = &TrapFrame)
    csrrw t6, sscratch, t6

    # 2. SAVE USER CONTEXT
    .set i, 0
    .rept 31
        save_gp %i
        .set i, i+1
    .endr

    # Save User t6 (from sscratch)
    csrr t0, sscratch
    sd   t0, (31*REG_SIZE + OFF_REGS)(t6)

    # Save FP Registers
    .set i, 0
    .rept 32
        save_fp %i
        .set i, i+1
    .endr

    # Save CSRs
    csrr t0, sepc
    sd   t0, OFF_SEPC(t6)
    csrr t1, sstatus
    sd   t1, OFF_SSTATUS(t6)
    csrr t2, scause
    sd   t2, OFF_SCAUSE(t6)
    csrr t3, stval
    sd   t3, OFF_STVAL(t6)

    # ---------------------------------------------------------
    # 3. PREPARE FOR KERNEL MODE
    # ---------------------------------------------------------
    # We are currently using the User Page Table. 0x1000 is valid.
    # The Kernel Page Table does NOT know 0x1000.
    # We must retrieve the Real Kernel Address BEFORE we switch.

    ld   t2, OFF_K_ADDR(t6)   # t2 = Real Pointer (0xFF... or 0x80...)

    # ---------------------------------------------------------
    # 4. SWAP SATP (Enter Kernel Mode)
    # ---------------------------------------------------------

    # A. Load the Stored Kernel SATP
    ld   t4, OFF_DUAL_SATP(t6)

    # B. Read the Current User SATP
    csrr t5, satp

    # C. Save User SATP to struct
    sd   t5, OFF_DUAL_SATP(t6)

    # D. Switch to Kernel Page Table
    beq  t4, t5, .skip_satp_switch
    csrw satp, t4
    sfence.vma
.skip_satp_switch:

    # ---------------------------------------------------------
    # 5. UPDATE BASE POINTER
    # ---------------------------------------------------------
    # Now that we (potentially) switched page tables, 0x1000 might be invalid.
    # We must use the Real Pointer we saved in t2.

    mv   t6, t2               # t6 is now the Real Kernel Address

    # ---------------------------------------------------------
    # 6. LOAD KERNEL CONTEXT
    # ---------------------------------------------------------
    ld   sp, OFF_K_SP(t6) # Load Kernel Stack
    ld   tp, OFF_K_TP(t6) # Load Kernel Thread Pointer

    # 7. CALL C++
    mv   a0, t6
    call handle_interrupt

    # Update t6 with potential new TrapFrame Real Pointer returned by C++
    mv   t6, a0

    # ---------------------------------------------------------
    # 8. RESTORE USER CONTEXT (Prepare to Return)
    # ---------------------------------------------------------
    # Restore CSRs
    ld   t0, OFF_SEPC(t6)
    csrw sepc, t0
    ld   t1, OFF_SSTATUS(t6)
    csrw sstatus, t1

    # ---------------------------------------------------------
    # 9. SWAP SATP (Return to User Mode)
    # ---------------------------------------------------------
    # Current State: SATP = Kernel SATP
    # t6 = Real Kernel Pointer

    # A. Load the Stored User SATP
    ld   t4, OFF_DUAL_SATP(t6)

    # B. Read the Current Kernel SATP
    csrr t5, satp

    # C. Save Kernel SATP to struct
    sd   t5, OFF_DUAL_SATP(t6)

    # D. Switch to User Page Table
    csrw satp, t4
    sfence.vma

    # ---------------------------------------------------------
    # 10. FORCE ALIAS (Critical Step!)
    # ---------------------------------------------------------
    # Now SATP is User. The Real Kernel Pointer (0xFF...) is invalid.
    # We must switch t6 to the Alias (0x1000) to access the frame.

    li   t6, TRAP_FRAME_ALIAS  # t6 = 0x1000

    # ---------------------------------------------------------
    # 11. RESTORE REGISTERS & EXIT
    # ---------------------------------------------------------
    .set i, 0
    .rept 32
        load_fp %i
        .set i, i+1
    .endr

    # Restore GP (SKIP t0/x5)
    .set i, 1
    .rept 30
        .if i != 5
            load_gp %i
        .endif
        .set i, i+1
    .endr

    # EXIT DANCE
    ld   t0, (31*REG_SIZE + OFF_REGS)(t6) # Load User t6 -> t0
    csrw sscratch, t0                     # User t6 -> sscratch
    ld   t0, (5*REG_SIZE + OFF_REGS)(t6)  # Restore User t0
    csrrw t6, sscratch, t6                # Swap: t6=UserT6, sscratch=&TF
    sret


# =============================================================
# SWITCH TO USER (Called from Scheduler)
# =============================================================
.global switch_to_user
switch_to_user:
    # a0 = Real Kernel Pointer to TrapFrame
    la   t0, m_trap_vector
    csrw stvec, t0

    # Enable interrupts (optional, usually handled by sstatus restore)
    li     t0, 1 << 1 | 1 << 5 | 1 << 9
    csrw    sie, t0

    mv   t6, a0

    # Restore CSRs
    ld   t0, OFF_SEPC(t6)
    csrw sepc, t0
    ld   t1, OFF_SSTATUS(t6)
    csrw sstatus, t1

    # ---------------------------------------------------------
    # SATP SWAP
    # ---------------------------------------------------------
    ld   t4, OFF_DUAL_SATP(t6)  # Load User SATP
    csrr t5, satp               # Read Kernel SATP
    sd   t5, OFF_DUAL_SATP(t6)  # Save Kernel SATP
    csrw satp, t4               # Switch to User SATP
    sfence.vma

    # ---------------------------------------------------------
    # FORCE ALIAS
    # ---------------------------------------------------------
    # Real Pointer is now invalid. Use 0x1000.
    li   t6, TRAP_FRAME_ALIAS

    # Restore Registers
    .set i, 0
    .rept 32
        load_fp %i
        .set i, i+1
    .endr

    .set i, 1
    .rept 30
        .if i != 5
            load_gp %i
        .endif
        .set i, i+1
    .endr

    ld   t0, (31*REG_SIZE + OFF_REGS)(t6)
    csrw sscratch, t0
    ld   t0, (5*REG_SIZE + OFF_REGS)(t6)
    csrrw t6, sscratch, t6
    sret