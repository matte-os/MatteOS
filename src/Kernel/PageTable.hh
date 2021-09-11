#pragma once

#include <Kernel/PageTableEntry.hh>
#include <Utils/DebugConsole.hh>

using Utils::DebugConsole;

namespace Kernel{
    struct PageTable{
        PageTableEntry entries[512];

        PageTable(){
            //entries = (PageTableEntry*)this;
        }

        void debugOutput();
    };
};