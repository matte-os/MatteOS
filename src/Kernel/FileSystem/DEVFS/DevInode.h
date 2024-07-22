#pragma once

#include <Kernel/FileSystem/Inode.h>
#include <Kernel/Forwards.h>
#include <Kernel/System/DeviceManager.h>
#include <Utils/Maps/HashMap.h>
#include <Utils/Strings/String.h>

namespace Kernel {

  using Utils::HashMap;
  using Utils::String;

  class DevInode : public Inode {
  private:
    RefPtr<Device> m_device;
    HashMap<String, RefPtr<Inode>> m_inodes;

  public:
  };

}// namespace Kernel
