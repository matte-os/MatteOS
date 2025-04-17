/**
 * @file Page.h
 * @author Matěj Bucek
 * This file is inspired by OSBlog.
 */
#pragma once
#include <Utils/Types.h>

namespace Kernel {
    enum class PageStatus : u8 {
        Free,
        Taken,
        Last
    };

    class Page{
        PageStatus m_status;
        public:
            Page();
            ~Page();
            bool is_last() const;
            bool is_taken() const;
            bool is_free() const;
            void clear();
            void set_status(PageStatus status);
    };
};