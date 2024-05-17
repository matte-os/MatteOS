#pragma once

#include <Kernel/Memory/PageTableEntry.h>
#include <Utils/DebugConsole.h>

using Utils::DebugConsole;

namespace Kernel::Memory {
    struct PageTable{
        PageTableEntry m_entries[512];

        PageTable(){
            //entries = (PageTableEntry*)this;
        }

        void debug_output();
    };
};