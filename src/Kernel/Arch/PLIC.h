/**
 * @file PLIC.h
 * @author Matěj Bucek
 */
#pragma once

#if ARCH == RISCV64
#include <Kernel/Arch/riscv64/Interrupts/Plic.h>
#endif