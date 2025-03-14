#pragma once

#include <Utils/Types.h>

namespace Kernel {
  enum SBIExtensionIDs {
    SBI_EXT_0_1_SET_TIMER = 0x0,
    SBI_EXT_0_1_CONSOLE_PUTCHAR = 0x1,
    SBI_EXT_0_1_CONSOLE_GETCHAR = 0x2,
    SBI_EXT_0_1_CLEAR_IPI = 0x3,
    SBI_EXT_0_1_SEND_IPI = 0x4,
    SBI_EXT_0_1_REMOTE_FENCE_I = 0x5,
    SBI_EXT_0_1_REMOTE_SFENCE_VMA = 0x6,
    SBI_EXT_0_1_REMOTE_SFENCE_VMA_ASID = 0x7,
    SBI_EXT_0_1_SHUTDOWN = 0x8,
    SBI_EXT_BASE = 0x10,
    SBI_EXT_TIME = 0x54494D45,
    SBI_EXT_IPI = 0x735049,
    SBI_EXT_RFENCE = 0x52464E43,
    SBI_EXT_HSM = 0x48534D,
    SBI_EXT_SRST = 0x53525354,
    SBI_EXT_PMU = 0x504D55,
    SBI_EXT_CONSOLE = 0x4442434E,
    /* Experimentals extensions must lie within this range */
    SBI_EXT_EXPERIMENTAL_START = 0x08000000,
    SBI_EXT_EXPERIMENTAL_END = 0x08FFFFFF,

    /* Vendor extensions must lie within this range */
    SBI_EXT_VENDOR_START = 0x09000000,
    SBI_EXT_VENDOR_END = 0x09FFFFFF,
  };

  struct SBIRet {
    long error;
    long value;
  };

  class SBI {
  public:
    static struct SBIRet sbi_ecall(int ext, int fid, unsigned long arg0,
                                   unsigned long arg1, unsigned long arg2,
                                   unsigned long arg3, unsigned long arg4,
                                   unsigned long arg5);
    static void sbi_console_putchar(int ch);
    static SBIRet sbi_remote_sfence_vma_asid(unsigned long hart_mask,
                                             unsigned long hart_mask_base,
                                             unsigned long start_addr,
                                             unsigned long size,
                                             unsigned long asid);
    static SBIRet sbi_hart_get_status(unsigned long hartid);
    static SBIRet sbi_hart_stop();
    static void sbi_set_timer(u64 stime_value);
    static SBIRet sbi_debug_console_write(unsigned long num_bytes,
                                          unsigned long base_addr_lo,
                                          unsigned long base_addr_hi);
    static SBIRet sbi_debug_console_read(unsigned long num_bytes,
                                        unsigned long base_addr_lo,
                                        unsigned long base_addr_hi);
    static SBIRet sbi_debug_console_write_byte(unsigned char byte);

  };
}// namespace Kernel