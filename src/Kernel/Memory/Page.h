#pragma once
#include <Utils/Types.h>

namespace Kernel {
    enum class PageStatus : u8 {
        FREE = 0,
        TAKEN = 1 << 0,
        LAST = 1 << 1
    };

    class Page{
        PageStatus m_status;
        public:
            Page();
            ~Page();
            bool is_last();
            bool is_taken();
            bool is_free();
            void clear();
            void set_status(PageStatus status);
    };
};