#pragma once

#include <Utils/Types.h>

namespace Kernel {
  struct BootSector {
    u8 jump[3];
    u8 oem[8];
    u16 bytes_per_sector;
    u8 sectors_per_cluster;
    u16 reserved_sectors;
    u8 fat_count;
    u16 root_entries;
    u16 total_sectors;
    u8 media;
    u16 sectors_per_fat;
    u16 sectors_per_track;
    u16 heads;
    u32 hidden_sectors;
    u32 total_sectors_large;
  } __attribute__((packed));

  struct BootSector32 : BootSector {
    u32 sectors_per_fat32;
    u16 flags;
    u16 version;
    u32 root_cluster;
    u16 fsinfo_sector;
    u16 backup_boot;
    u8 reserved[12];
    u8 drive_number;
    u8 reserved1;
    u8 signature;
    u32 volume_id;
    u8 volume_label[11];
    u8 system_id[8];
  } __attribute__((packed));

  struct FATEntry {
    u32 value    : 28;
    u32 reserved : 4;
  } __attribute__((packed));

  struct DirectoryEntry {
    u8 name[11];
    u8 attributes;
    u8 reserved;
    u8 creation_time_tenths;
    u16 creation_time;
    u16 creation_date;
    u16 last_access_date;
    u16 first_cluster_high;
    u16 last_write_time;
    u16 last_write_date;
    u16 first_cluster_low;
    u32 size;
  } __attribute__((packed));

  struct LongDirectoryEntry {
    u8 order;
    u8 name1[10];
    u8 attributes;
    u8 type;
    u8 checksum;
    u8 name2[12];
    u16 first_cluster;
    u8 name3[4];
  } __attribute__((packed));

  struct FAT32FSInfo {
    u32 signature1;
    u8 reserved[480];
    u32 signature2;
    u32 free_clusters;
    u32 next_free_cluster;
    u8 reserved1[12];
    u32 signature3;
  } __attribute__((packed));
}// namespace Kernel