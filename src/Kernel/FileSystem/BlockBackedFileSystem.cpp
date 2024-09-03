#include <Kernel/FileSystem/BlockBackedFileSystem.h>

namespace Kernel {
  ErrorOr<RefPtr<DataBlock>> BlockBackedFileSystem::read_block_poll(size_t block_number, size_t size) {
  }

  ErrorOr<void> BlockBackedFileSystem::write_block_poll(RefPtr<DataBlock> block) {
  }

  ErrorOr<void> BlockBackedFileSystem::read_request() {
  }

  ErrorOr<void> BlockBackedFileSystem::write_request() {
  }
}// namespace Kernel