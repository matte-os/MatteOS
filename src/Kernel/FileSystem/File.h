#pragma once

#include <Kernel/FileSystem/FileDescriptor.h>
#include <Kernel/FileSystem/Inode.h>

namespace Kernel {
  class File : public FileDescriptor {
  private:
    RefPtr<Inode> m_inode;
    size_t m_offset;
    FileOpenMode m_mode;

  public:
    explicit File(RefPtr<Inode> inode, FileOpenMode mode) : m_inode(inode), m_offset(0), m_mode(mode) {}
    virtual ~File() = default;

    virtual bool can_read() const override;
    virtual bool can_write() const override;

    virtual size_t read(u8* buffer, size_t size, size_t offset) override;
    virtual size_t write(const u8* buffer, size_t size, size_t offset) override;

    virtual void close() override;
  };
}// namespace Kernel
