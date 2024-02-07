#pragma once

#include "TrapFrame.h"
#include "Utils/Types.hh"

using Kernel::TrapFrame;

extern "C" size_t trap_vector(
        u64 epc,
        u64 tval,
        u64 cause,
        u64 hart,
        u64 status,
        TrapFrame *frame);