#pragma once

#include <Kernel/Devices/Storage/BlockDevice.h>
#include <Kernel/FileSystem/FileSystem.h>
#include <Utils/Arrays/Array.h>

namespace Kernel {
  class DataBlock : RefCounted<DataBlock> {
  private:
    u8* m_data;
    size_t m_size;
    size_t m_block_number;

  public:
    DataBlock(size_t block_number, size_t size) : m_data(new u8[size]), m_size(size), m_block_number(block_number) {}

    ~DataBlock() {
      delete[] m_data;
    }

    u8* data() {
      return m_data;
    }

    size_t size() {
      return m_size;
    }

    size_t block_number() {
      return m_block_number;
    }
  };

  // This is a backbone for block-based file systems.
  // It will automatically cache blocks and provide a way to read and write them.
  class BlockBackedFileSystem : public FileSystem {
  private:
    RefPtr<BlockDevice> m_device;

  protected:
    BlockBackedFileSystem(RefPtr<BlockDevice> device) : m_device(device) {};
    ErrorOr<RefPtr<DataBlock>> read_block_poll(size_t block_number, size_t size);
    ErrorOr<void> write_block_poll(RefPtr<DataBlock> block);
    ErrorOr<void> read_request();
    ErrorOr<void> write_request();
    RefPtr<BlockDevice> get_device();
  };
}// namespace Kernel
