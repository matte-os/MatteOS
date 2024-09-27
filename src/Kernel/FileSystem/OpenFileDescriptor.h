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

  class OpenFileDescriptor : public RefCounted<OpenFileDescriptor> {
  private:
    RefPtr<Inode> m_inode;

  public:
    OpenFileDescriptor(RefPtr<Inode> inode) : m_inode(move(inode)) {}

    RefPtr<Inode> inode() {
      return m_inode;
    }

    bool close();
  };

  class OpenFileDescriptorTable {
  private:
    HashMap<String, RefPtr<OpenFileDescriptor>> m_descriptors;

  public:
    bool has_descriptor(const String& name) const {
      return m_descriptors.has_key(name);
    }

    ErrorOr<RefPtr<OpenFileDescriptor>> get_descriptor(const String& name) const;
    ErrorOr<void> open(RefPtr<OpenFileDescriptor> descriptor);
    ErrorOr<void> close(const String& name);
  };
}// namespace Kernel
