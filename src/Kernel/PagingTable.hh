#pragma once
#include <Utils/Types.hh>

namespace Kernel{
    enum EntryBits : __int64{
        NONE = 0,
        VALID = 1 << 0,
        READ = 1 << 1,
        WRITE = 1 << 2,
        EXECUTE = 1 << 3,
        USER = 1 << 4,
        GLOBAL = 1 << 5,
        ACCESS = 1 << 6,
        DIRTY = 1 << 7,

        READ_WRITE = 1 << 1 | 1 << 2,
        READ_EXECUTE = 1 << 1 | 1 << 3,
        READ_WRITE_EXECUTE = 1 << 1 | 1 << 2 | 1 << 3,

        USER_READ_WRITE = 1 << 1 | 1 << 2 | 1 << 4,
        USER_READ_EXECUTE = 1 << 1 | 1 << 3 | 1 << 4,
        USER_READ_WRITE_EXECUTE = 1 << 1 | 1 << 2 | 1 << 3 | 1 << 4
    };

    class TableEntry{
        __int64 entry;
    public:
        TableEntry(){};
        bool isValid();
        bool isInvalid();
        bool isLeaf();
        bool isBranch();
        void setEntry(__int64);
        __int64 getEntry();
    };

    class PagingTable{
    public:
        TableEntry entries[512];
        static int length();
    };

    void map(PagingTable &, size_t, size_t, __int64, size_t);

    void umap(PagingTable &);
};