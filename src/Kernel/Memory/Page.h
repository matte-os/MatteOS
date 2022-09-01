#pragma once
#include <Utils/Types.hh>

namespace Kernel::Memory {
    enum class PageStatus : u8 {
        FREE = 0,
        TAKEN = 1 << 0,
        LAST = 1 << 1
    };

    class Page{
        PageStatus status;
        public:
            Page();
            ~Page();
            bool isLast();
            bool isTaken();
            bool isFree();
            void clear();
            void setStatus(PageStatus);
    };
};