#pragma once

#include <Kernel/FileSystem/Inode.h>
#include <Utils/Maps/HashMap.h>
#include <Utils/Strings/String.h>

namespace Kernel {
    using Utils::HashMap;
    using Utils::String;

    class RamInode : public Inode {
        HashMap<String, RefPtr<Inode> > m_children;
        RefPtr<RamFileSystem> m_fs;

    protected:
        enum class Type {
            Ram,
            Device
        } m_type = Type::Ram;

    public:
        explicit RamInode(const Type type) : m_type(type) {}
        ~RamInode() override = default;

        ErrorOr<size_t> read(u8* buffer, size_t size, size_t offset) override;
        ErrorOr<size_t> write(const u8* buffer, size_t size, size_t offset) override;
        RefPtr<FileSystem> fs() override;
    };

    class DevInode : public RamInode {
        RefPtr<Device> m_device;

    public:
        explicit DevInode(const RefPtr<Device>& device) : RamInode(Type::Device), m_device(device) {}
        ~DevInode() override = default;
    };
} // namespace Kernel
