#pragma once
#include <Utils/Types.hh>

namespace Kernel{
    enum PageBits : u8{
        EMPTY = 0,
        TAKEN = 1 << 0,
        LAST = 1 << 1
    };

    class Page {
        u8 flags;
    public:
        Page();
        Page(PageBits flag);
        bool isLast();
        bool isTaken();
        bool isFree();
        void clear();
        void setFlag(PageBits flag);
    };
};