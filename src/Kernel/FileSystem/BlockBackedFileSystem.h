#pragma once

#include <Kernel/Devices/Storage/BlockDevice.h>
#include <Kernel/FileSystem/FileSystem.h>
#include <Utils/Arrays/LinkedList.h>

namespace Kernel {
  using Utils::LinkedList;

  class DataBlock : public RefCounted<DataBlock> {
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
    LinkedList<RefPtr<DataBlock>> m_cache;
    size_t m_block_size;

  protected:
    static const size_t MaxCacheSize = 16;

    BlockBackedFileSystem(RefPtr<BlockDevice> device, size_t block_size) : m_device(device), m_block_size(block_size) {};

    // Cache methods
    void cache(RefPtr<DataBlock> block) {
      if(get_cache_size() >= MaxCacheSize) {
        evict_block();
      }

      m_cache.prepend(block);
    }

    void evict_block() {
      // TODO: Implement this

      // Get last

      // Check if it's dirty

      // Write it back to the device

      // Remove it from the cache
    }

    constexpr size_t get_cache_size() {
      return m_cache.size();
    }

    ErrorOr<RefPtr<DataBlock>> get_block_from_cache(size_t block_number) {
      auto maybe_block = m_cache.find_first_match([block_number](auto& block) {
        return block->block_number() == block_number;
      });

      if(maybe_block) {
        return maybe_block.get_value();
      }

      return ErrorOr<RefPtr<DataBlock>>::create_error(Error::create_from_string("Block not found"));
    }

    // Block device methods
    virtual ErrorOr<RefPtr<DataBlock>> read_block_poll(size_t block_number) = 0;
    virtual ErrorOr<void> write_block_poll(size_t block_number, char* buffer) = 0;
    virtual ErrorOr<void> read_block_request() = 0;
    virtual ErrorOr<void> write_block_request() = 0;
    RefPtr<BlockDevice> get_device();

  public:
    constexpr size_t get_block_size() {
      return m_block_size;
    }
  };
}// namespace Kernel
