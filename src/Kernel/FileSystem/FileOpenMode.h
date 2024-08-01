#pragma once

#include <Utils/Types.h>

namespace Kernel {
  enum class FileAccessMode : u8 {
    Read = 0,
    Write,
    ReadWrite,
  };

  enum class FileOperationMode : u8 {
    Create = 0 << 3,
    Append = 1 << 3,
    Exclusive = 2 << 3,
    Truncate = 3 << 3,
  };

  class FileOpenMode {
  private:
    size_t m_mode;

  public:
    constexpr FileOpenMode(size_t mode) : m_mode(mode) {}
    constexpr FileOpenMode() : m_mode(0) {}
    bool is_read() const { return m_mode & 1; }
    bool is_write() const { return m_mode & 2; }
    bool is_read_write() const { return m_mode & 3; }
    bool is_create() const { return m_mode & 8; }
    bool is_append() const { return m_mode & 16; }
    bool is_exclusive() const { return m_mode & 32; }
    bool is_truncate() const { return m_mode & 64; }
    template<FileAccessMode Mode>
    bool is() const { return m_mode & static_cast<size_t>(Mode); }
    template<FileOperationMode Mode>
    bool is() const { return m_mode & static_cast<size_t>(Mode); }
  };

  template<typename... Modes>
  constexpr FileOpenMode build_file_open_mode(Modes... modes) {
    return (static_cast<FileOpenMode>(modes) | ...);
  }
}// namespace Kernel