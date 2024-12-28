#pragma once

#include <Kernel/FileSystem/FileOpenMode.h>
#include <Kernel/Forwards.h>
#include <Kernel/Security/Credentials.h>
#include <Utils/Arrays/ArrayList.h>
#include <Utils/Errors/ErrorOr.h>
#include <Utils/Pointers/RefCounted.h>
#include <Utils/Strings/String.h>
#include <Utils/Types.h>

namespace Kernel {
  using Utils::ArrayList;
  using Utils::ErrorOr;
  using Utils::String;
  using Utils::StringView;

  class DirectoryEntry;

  class Inode : public RefCounted<Inode> {
  protected:
    Inode() = default;

  public:
    virtual ~Inode() = default;

    virtual ErrorOr<void> check_credentials(const Credentials& credentials, const FileOpenMode& mode) = 0;

    virtual ErrorOr<size_t> read(u8* buffer, size_t size, size_t offset) = 0;
    virtual ErrorOr<size_t> write(const u8* buffer, size_t size, size_t offset) = 0;
    virtual RefPtr<FileSystem> fs() = 0;

    virtual ErrorOr<RefPtr<Inode>> get_child(StringView name) = 0;
    virtual ErrorOr<ArrayList<RefPtr<DirectoryEntry>>> list_dir() = 0;
    virtual ErrorOr<StringView> name() = 0;

    virtual bool is_directory() = 0;
    virtual bool is_file() = 0;
  };

  class DirectoryEntry : public RefCounted<DirectoryEntry> {
  private:
    String m_name;
  public:
    DirectoryEntry(String name) : m_name(name) {}

    virtual ~DirectoryEntry() = default;

    StringView get_name() {
      return m_name;
    }
  };
}// namespace Kernel
