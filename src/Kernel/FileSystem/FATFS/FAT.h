/**
 * @file FAT.h
 * @author Matěj Bucek
 *
 * This file is based on the FAT32 specification.
 */
#pragma once

#include <Utils/Traits.h>
#include <Utils/Types.h>

namespace Kernel {
  using Utils::as_underlying;

  namespace FAT {
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

    static_assert(sizeof(BootSector) == 36, "BootSector size is not 36 bytes");

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

    static_assert(sizeof(BootSector32) == 90, "BootSector32 size is not 90 bytes");

    struct FATEntry {
      u32 value    : 28;
      u32 reserved : 4;

      bool is_end_of_chain() {
        return value >= 0x0FFFFFF8;
      }

      bool is_free() {
        return value == 0x00000000;
      }
    } __attribute__((packed));

    static_assert(sizeof(FATEntry) == 4, "FATEntry size is not 4 bytes");

    enum class DirectoryEntryAttributes : u8 {
      ReadOnly = 0x01,
      Hidden = 0x02,
      System = 0x04,
      VolumeID = 0x08,
      Directory = 0x10,
      Archive = 0x20,
      LongName = ReadOnly | Hidden | System | VolumeID,
    };

    struct LongDirectoryEntry {
      u8 order;
      u16 name1[5];
      u8 attributes;
      u8 type;
      u8 checksum;
      u16 name2[6];
      u16 first_cluster;
      u16 name3[2];

      bool is_final_entry() {
        return order & 0x40;
      }

      bool is_free() {
        return order & 0x00;
      }
    } __attribute__((packed));

    static_assert(sizeof(LongDirectoryEntry) == 32, "LongDirectoryEntry size is not 32 bytes");

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

      bool is_free() {
        return name[0] == 0xE5 || name[0] == 0x00;
      }

      bool is_directory() {
        return attributes & as_underlying(DirectoryEntryAttributes::Directory);
      }

      bool is_long_name() {
        return attributes & as_underlying(DirectoryEntryAttributes::LongName) == as_underlying(DirectoryEntryAttributes::LongName);
      }

      bool is_volume_id() {
        return attributes & as_underlying(DirectoryEntryAttributes::VolumeID);
      }

      bool is_system() {
        return attributes & as_underlying(DirectoryEntryAttributes::System);
      }

      bool is_hidden() {
        return attributes & as_underlying(DirectoryEntryAttributes::Hidden);
      }

      bool is_read_only() {
        return attributes & as_underlying(DirectoryEntryAttributes::ReadOnly);
      }

      bool is_archive() {
        return attributes & as_underlying(DirectoryEntryAttributes::Archive);
      }

      LongDirectoryEntry* as_long_name() {
        return reinterpret_cast<LongDirectoryEntry*>(this);
      }
    } __attribute__((packed));

    static_assert(sizeof(DirectoryEntry) == 32, "DirectoryEntry size is not 32 bytes");

    struct FAT32FSInfo {
      u32 signature1;
      u8 reserved[480];
      u32 signature2;
      u32 free_clusters;
      u32 next_free_cluster;
      u8 reserved1[12];
      u32 signature3;
    } __attribute__((packed));

    struct FullDirectoryEntry {
      String name;
      u32 size;
      u32 first_cluster;
      u8 attributes;
      u8 creation_time_tenths;
      u16 creation_time;
      u16 creation_date;
      u16 last_access_date;
      u16 last_write_time;
      u16 last_write_date;

      static FullDirectoryEntry filled_from(String name, DirectoryEntry& entry) {
        return {
          .name = name,
          .size = entry.size,
          .first_cluster = (u32(entry.first_cluster_high) << 16) | entry.first_cluster_low,
          .attributes = entry.attributes,
          .creation_time_tenths = entry.creation_time_tenths,
          .creation_time = entry.creation_time,
          .creation_date = entry.creation_date,
          .last_access_date = entry.last_access_date,
          .last_write_time = entry.last_write_time,
          .last_write_date = entry.last_write_date,
        };
      }

      bool is_free() {
        return name[0] == 0xE5 || name[0] == 0x00;
      }

      bool is_directory() {
        return attributes & as_underlying(DirectoryEntryAttributes::Directory);
      }

      bool is_long_name() {
        return attributes & as_underlying(DirectoryEntryAttributes::LongName) == as_underlying(DirectoryEntryAttributes::LongName);
      }

      bool is_volume_id() {
        return attributes & as_underlying(DirectoryEntryAttributes::VolumeID);
      }

      bool is_system() {
        return attributes & as_underlying(DirectoryEntryAttributes::System);
      }

      bool is_hidden() {
        return attributes & as_underlying(DirectoryEntryAttributes::Hidden);
      }

      bool is_read_only() {
        return attributes & as_underlying(DirectoryEntryAttributes::ReadOnly);
      }

      bool is_archive() {
        return attributes & as_underlying(DirectoryEntryAttributes::Archive);
      }
    };
  }// namespace FAT
}// namespace Kernel