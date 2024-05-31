//
// Created by matejbucek on 1.9.22.
//

#pragma once
#include <Utils/Arrays/ArrayList.h>
#include <Utils/Endian.h>
#include <Utils/Strings/String.h>
#include <Utils/Types.h>

using Utils::ArrayList;
using Utils::Endian;
using Utils::Endianness;
using Utils::Strings::String;

struct FDTHeader {
  Endian<u32, Endianness::Big> magic;
  Endian<u32, Endianness::Big> totalsize;
  Endian<u32, Endianness::Big> off_dt_struct;
  Endian<u32, Endianness::Big> off_dt_strings;
  Endian<u32, Endianness::Big> off_mem_rsvmap;
  Endian<u32, Endianness::Big> version;
  Endian<u32, Endianness::Big> last_comp_version;
  Endian<u32, Endianness::Big> boot_cpuid_phys;
  Endian<u32, Endianness::Big> size_dt_strings;
  Endian<u32, Endianness::Big> size_dt_struct;
};

struct FDTReserveEntry {
  Endian<u64, Endianness::Big> address;
  Endian<u64, Endianness::Big> size;
};

struct FDTProp {
  Endian<u32, Endianness::Big> len;
  Endian<u32, Endianness::Big> name_offset;
};

struct FDTProperty {
  String name;
  String value;
};

struct FDTNode {
  String name;
  ArrayList<FDTProperty> properties;
  ArrayList<FDTNode*> children;
};



enum class FDTNodeType : u32 {
  FDT_BEGIN_NODE = 0x00000001,
  FDT_END_NODE = 0x00000002,
  FDT_PROP = 0x00000003,
  FDT_NOP = 0x00000004,
  FDT_END = 0x00000009
};
