#include <Kernel/SBI/SBI.h>

namespace Kernel {
    SBIRet SBI::sbi_ecall(int ext, int fid, unsigned long arg0,
                          unsigned long arg1, unsigned long arg2,
                          unsigned long arg3, unsigned long arg4,
                          unsigned long arg5) {
        SBIRet ret{};

        register auto a0 asm("a0") = (uintptr_t) (arg0);
        register auto a1 asm("a1") = (uintptr_t) (arg1);
        register auto a2 asm("a2") = (uintptr_t) (arg2);
        register auto a3 asm("a3") = (uintptr_t) (arg3);
        register auto a4 asm("a4") = (uintptr_t) (arg4);
        register auto a5 asm("a5") = (uintptr_t) (arg5);
        register auto a6 asm("a6") = (uintptr_t) (fid);
        register auto a7 asm("a7") = (uintptr_t) (ext);
        asm volatile("ecall"
            : "+r"(a0), "+r"(a1)
            : "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6), "r"(a7)
            : "memory");
        ret.error = a0;
        ret.value = a1;

        return ret;
    }

    void SBI::sbi_console_putchar(int ch) {
        sbi_ecall(SBI_EXT_0_1_CONSOLE_PUTCHAR, 0, ch, 0, 0, 0, 0, 0);
    }


    SBIRet
    SBI::sbi_remote_sfence_vma_asid(unsigned long hart_mask, unsigned long hart_mask_base, unsigned long start_addr,
                                    unsigned long size, unsigned long asid) {
        return sbi_ecall(SBI_EXT_RFENCE, 2, hart_mask, hart_mask_base, start_addr, size, asid, 0);
    }

    SBIRet SBI::sbi_hart_get_status(unsigned long hartid) {
        return sbi_ecall(SBI_EXT_HSM, 2, hartid, 0, 0, 0, 0, 0);
    }

    void SBI::sbi_set_timer(const u64 stime_value) {
        sbi_ecall(SBI_EXT_TIME, 0, stime_value, 0, 0, 0, 0, 0);
    }

    SBIRet SBI::sbi_debug_console_write(unsigned long num_bytes, unsigned long base_addr_lo,
                                        unsigned long base_addr_hi) {
        return sbi_ecall(SBI_EXT_CONSOLE, 0, num_bytes, base_addr_lo, base_addr_hi, 0, 0, 0);
    }

    SBIRet SBI::sbi_debug_console_read(unsigned long num_bytes, unsigned long base_addr_lo,
                                       unsigned long base_addr_hi) {
        return sbi_ecall(SBI_EXT_CONSOLE, 1, num_bytes, base_addr_lo, base_addr_hi, 0, 0, 0);
    }

    SBIRet SBI::sbi_debug_console_write_byte(unsigned char byte) {
        return sbi_ecall(SBI_EXT_CONSOLE, 2, byte, 0, 0, 0, 0, 0);
    }
} // namespace Kernel
