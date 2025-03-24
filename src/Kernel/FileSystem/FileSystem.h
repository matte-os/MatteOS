#pragma once

#include <Kernel/FileSystem/Inode.h>
#include <Kernel/FileSystem/OpenFileDescriptor.h>
#include <Utils/Strings/String.h>

namespace Kernel {
  using Utils::String;

  class FileSystem : public RefCounted<FileSystem> {
  protected:
    OpenFileDescriptorTable m_open_files;

  public:
    FileSystem() = default;
    virtual ~FileSystem() = default;

    virtual String name() const = 0;
    virtual ErrorOr<RefPtr<Inode>> root() = 0;
    virtual ErrorOr<RefPtr<OpenFileDescriptor>> open(const Credentials& credentials, StringView path, FileOpenMode mode);
    virtual bool exists(StringView path) = 0;
    virtual ErrorOr<void> close(RefPtr<OpenFileDescriptor> inode) = 0;
  };
}// namespace Kernel