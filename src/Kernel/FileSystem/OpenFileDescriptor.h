#pragma once

#include <Kernel/FileSystem/Inode.h>
#include <Utils/Errors/ErrorOr.h>
#include <Utils/Maps/HashMap.h>
#include <Utils/Pointers/RefCounted.h>

namespace Kernel {
  using Utils::Error;
  using Utils::ErrorOr;
  using Utils::HashMap;
  using Utils::RefCounted;

  class OpenFileDescriptorTable;

  class OpenFileDescriptor : public RefCounted<OpenFileDescriptor> {
  private:
    friend class OpenFileDescriptorTable;

    RefPtr<Inode> m_inode;
    size_t m_number_of_openers;

    void open();
    bool close();

  public:
    OpenFileDescriptor(RefPtr<Inode> inode) : m_inode(move(inode)), m_number_of_openers(0) {}

    RefPtr<Inode> inode() {
      return m_inode;
    }
  };

  class OpenFileDescriptorTable {
  private:
    HashMap<String, RefPtr<OpenFileDescriptor>> m_descriptors;

  public:
    bool has_descriptor(const String& name) const {
      return m_descriptors.has_key(name);
    }

    ErrorOr<RefPtr<OpenFileDescriptor>> create_descriptor_and_open(const String& path, RefPtr<Inode> inode);
    ErrorOr<RefPtr<OpenFileDescriptor>> open(const String& path);
    ErrorOr<void> close(const String& path);
  };
}// namespace Kernel
