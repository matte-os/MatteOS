#pragma once

#include <Utils/Types.h>

namespace Kernel::ELF {

  // =============================================================
  // 1. Strongly Typed Enums (The "IDE Helper" part)
  // =============================================================

  enum class FileType : u16 {
    None = 0,
    Relocatable = 1, // .ko (Modules)
    Executable = 2,  // .elf (Processes)
    Dynamic = 3,     // .so (Shared Libraries)
    Core = 4
  };

  enum class Machine : u16 {
    None = 0,
    X86_64 = 62,
    RISCV = 243
  };

  enum class SegmentType : u32 {
    Null = 0,
    Load = 1,    // The one you care about
    Dynamic = 2,
    Interp = 3,
    Note = 4,
    ShLib = 5,
    Phdr = 6,
    TLS = 7
  };

  enum class SegmentFlags : u32 {
    Execute = 1,
    Write = 2,
    Read = 4,
    ReadWrite = 6,
    ReadExecute = 5,
    ReadWriteExecute = 7
  };

  enum class SectionType : u32 {
    Null = 0,
    ProgBits = 1, // Code or Data
    SymTab = 2,   // Symbol Table
    StrTab = 3,   // String Table
    Rela = 4,     // Relocations (with addend)
    Hash = 5,
    Dynamic = 6,
    Note = 7,
    NoBits = 8,   // BSS
    Rel = 9,
    ShLib = 10,
    DynSym = 11,
    RiscvAttributes = 0x70000003 // RISC-V specific
  };

  enum class SectionFlags : u64 {
    Write = (1 << 0),
    Alloc = (1 << 1),
    ExecInstr = (1 << 2),
    Merge = (1 << 4),
    Strings = (1 << 5)
  };

  // RISC-V Relocation Types
  // (Used inside Relocation::type())
  enum class RelocationType : u32 {
    None = 0,
    Riscv32 = 1,
    Riscv64 = 2,
    Relative = 3,
    Copy = 4,
    JumpSlot = 5,
    TlsDtpMod64 = 7,
    TlsDtpRel64 = 9,
    TlsTpRel64 = 11,
    Branch = 16,
    Jal = 17,
    Call = 18,
    CallPlt = 19,
    GotHi20 = 20,
    PcrelHi20 = 23,
    PcrelLo12I = 24,
    PcrelLo12S = 25,
    Hi20 = 26,
    Lo12I = 27,
    Lo12S = 28,
    TpRelHi20 = 29,
    TpRelLo12I = 30,
    TpRelLo12S = 31,
    Add64 = 36,
    Sub64 = 40,
  };

  // Symbol Types (Extract from Symbol::info)
  enum class SymbolType : u8 {
    NoType = 0,
    Object = 1,  // Variable
    Func = 2,    // Function
    Section = 3,
    File = 4
  };

  enum class SymbolBinding : u8 {
    Local = 0,
    Global = 1,
    Weak = 2
  };

  // =============================================================
  // 2. The Structs (Binary Layout)
  // =============================================================

  using Elf64_Addr = u64;
  using Elf64_Off = u64;
  using Elf64_Word = u32;
  using Elf64_Xword = u64;
  using Elf64_Sxword = s64;

  struct ElfHeader {
    u8 ident[16];
    FileType type;       // Now an Enum (u16)
    Machine machine;     // Now an Enum (u16)
    Elf64_Word version;
    Elf64_Addr entry;
    Elf64_Off program_header_offset;
    Elf64_Off section_header_offset;
    Elf64_Word flags;
    u16 elf_header_size;
    u16 program_header_entry_size;
    u16 program_header_entry_count;
    u16 section_header_entry_size;
    u16 section_header_entry_count;
    u16 section_header_string_table_index;
  } __attribute__((packed));

  static_assert(sizeof(ElfHeader) == 64, "ElfHeader size mismatch");


  struct ProgramHeader {
    SegmentType type;    // Now an Enum (u32)
    u32 flags;           // Kept as u32 because it's a bitmask (OR-able)
    Elf64_Off offset;
    Elf64_Addr virtual_address;
    Elf64_Addr physical_address;
    Elf64_Xword in_file_size;
    Elf64_Xword in_memory_size;
    Elf64_Xword align;

    // Helper to check flags nicely
    [[nodiscard]] bool is_readable() const { return flags & (u32)SegmentFlags::Read; }
    [[nodiscard]] bool is_writable() const { return flags & (u32)SegmentFlags::Write; }
    [[nodiscard]] bool is_executable() const { return flags & (u32)SegmentFlags::Execute; }
  } __attribute__((packed));

  static_assert(sizeof(ProgramHeader) == 56, "ProgramHeader size mismatch");


  struct SectionHeader {
    Elf64_Word name_index;
    SectionType type;    // Now an Enum (u32)
    u64 flags;           // Kept as u64 because it's a bitmask
    Elf64_Addr addr;
    Elf64_Off offset;
    Elf64_Xword size;
    Elf64_Word link;
    Elf64_Word info;
    Elf64_Xword align;
    Elf64_Xword entry_size;

    // Helper for flags
    [[nodiscard]] bool is_writable() const { return flags & (u64)SectionFlags::Write; }
    [[nodiscard]] bool is_alloc() const { return flags & (u64)SectionFlags::Alloc; }
    [[nodiscard]] bool is_executable() const { return flags & (u64)SectionFlags::ExecInstr; }
  } __attribute__((packed));

  static_assert(sizeof(SectionHeader) == 64, "SectionHeader size mismatch");


  struct Symbol {
    Elf64_Word name_index;
    u8 info;     // Packed Binding (high 4 bits) + Type (low 4 bits)
    u8 other;
    u16 section_index;
    Elf64_Addr value;
    Elf64_Xword size;

    // --- Helpers to unpack 'info' ---
    [[nodiscard]] SymbolBinding binding() const { return static_cast<SymbolBinding>(info >> 4); }
    [[nodiscard]] SymbolType type() const { return static_cast<SymbolType>(info & 0xF); }
  } __attribute__((packed));

  static_assert(sizeof(Symbol) == 24, "Symbol size mismatch");


  struct Relocation {
    Elf64_Addr offset;
    Elf64_Xword info;   // Packed Symbol Index (high 32) + Type (low 32)
    Elf64_Sxword addend;

    // --- Helpers to unpack 'info' ---
    [[nodiscard]] u32 symbol_index() const { return static_cast<u32>(info >> 32); }
    [[nodiscard]] RelocationType type() const { return static_cast<RelocationType>(info & 0xFFFFFFFF); }
  } __attribute__((packed));

  static_assert(sizeof(Relocation) == 24, "Relocation size mismatch");

} // namespace Kernel::ELF