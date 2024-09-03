#pragma once

#include <Utils/Endian.h>

#define VIRTIO_BLK_T_IN 0
#define VIRTIO_BLK_T_OUT 1
#define VIRTIO_BLK_T_FLUSH 4
#define VIRTIO_BLK_T_DISCARD 11
#define VIRTIO_BLK_T_WRITE_ZEROES 13
#define VIRTIO_BLK_S_OK 0
#define VIRTIO_BLK_S_IOERR 1
#define VIRTIO_BLK_S_UNSUPP 2

namespace Kernel {

  using Utils::Endian;
  using Utils::Endianness;

  namespace BlockIO {
    struct Config {
      Endian<u64, Endianness::Little> capacity;
      Endian<u32, Endianness::Little> size_max;
      Endian<u32, Endianness::Little> seg_max;

      struct Geometry {
        Endian<u16, Endianness::Little> cylinders;
        u8 heads;
        u8 sectors;
      } geometry;

      Endian<u32, Endianness::Little> blk_size;

      struct Topology {
        u8 physical_block_exp;
        u8 alignment_offset;
        Endian<u16, Endianness::Little> min_io_size;
        Endian<u32, Endianness::Little> opt_io_size;
      } topology;

      u8 writeback;
      u8 unused1[3];
      Endian<u32, Endianness::Little> max_discard_sectors;
      Endian<u32, Endianness::Little> max_discard_seg;
      Endian<u32, Endianness::Little> discard_sector_alignment;
      Endian<u32, Endianness::Little> max_write_zeroes_sectors;
      Endian<u32, Endianness::Little> max_write_zeroes_seg;
      u8 write_zeroes_may_unmap;
      u8 unused2[3];
    };

    enum class RequestType : u32 {
      Read = VIRTIO_BLK_T_IN,
      Write = VIRTIO_BLK_T_OUT,
      Flush = VIRTIO_BLK_T_FLUSH,
      Discard = VIRTIO_BLK_T_DISCARD,
      WriteZeroes = VIRTIO_BLK_T_WRITE_ZEROES,
    };

    struct Header {
      Endian<u32, Endianness::Little> type;
      Endian<u32, Endianness::Little> reserved;
      Endian<u64, Endianness::Little> sector;
    };

    struct Request {
      Header header;
      u8* data;
      u8 status;
      s64 watcher_pid;
    };

    struct DiscardWriteZeroes {
      Endian<u64, Endianness::Little> sector;
      Endian<u32, Endianness::Little> num_sectors;
      Endian<u32, Endianness::Little> flags;
    };
  }// namespace BlockIO
}// namespace Kernel
