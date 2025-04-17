/**
 * @file BlockBackedFileSystem.cpp
 * @author Matěj Bucek
 */
#include <Kernel/FileSystem/BlockBackedFileSystem.h>

namespace Kernel {
  RefPtr<BlockDevice> BlockBackedFileSystem::get_device() {
    return m_device;
  }
}// namespace Kernel