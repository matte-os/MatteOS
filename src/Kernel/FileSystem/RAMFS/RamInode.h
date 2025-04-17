/**
 * @file RamInode.h
 * @author Matěj Bucek
 */
#pragma once

#include <Kernel/FileSystem/Inode.h>
#include <Utils/Maps/HashMap.h>
#include <Utils/Strings/String.h>

namespace Kernel {
    using Utils::HashMap;
    using Utils::String;

    class RamInode : public Inode {
        friend class DeviceManager;
        HashMap<String, RefPtr<Inode>> m_children;
        RefPtr<RamFileSystem> m_fs;

    protected:
        enum class Type {
            RamDirectory,
            RamFile,
            Device
        } m_type = Type::RamDirectory;

    public:
        explicit RamInode(const Type type) : m_type(type) {}
        ~RamInode() override = default;

        static RefPtr<RamInode> create_directory();

        virtual ErrorOr<Request<size_t>> read(u8* buffer, size_t size, size_t offset) override;
        ErrorOr<size_t> write(const u8* buffer, size_t size, size_t offset) override;
        RefPtr<FileSystem> fs() override;
        ErrorOr<void> check_credentials(const Credentials& credentials, const FileOpenMode& mode) override;
        ErrorOr<RefPtr<Inode>> get_child(StringView name) override;
        ErrorOr<ArrayList<RefPtr<DirectoryEntry>>> list_dir() override;
        ErrorOr<StringView> name() override;
        bool is_directory() override;
        bool is_file() override;
    };

    class DevInode : public RamInode {
        friend class DeviceManager;
        RefPtr<Device> m_device;

    public:
        explicit DevInode(const RefPtr<Device>& device) : RamInode(Type::Device), m_device(device) {}
        ~DevInode() override = default;

      bool is_directory() override;
      bool is_file() override;

      virtual ErrorOr<Request<size_t>> read(u8* buffer, size_t size, size_t offset) override;
    };
} // namespace Kernel
