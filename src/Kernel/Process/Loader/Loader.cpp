#include <Kernel/Process/Loader/Loader.h>

#include <Kernel/API/Module/Module.h>
#include <Kernel/Arch/riscv64/CPU.h>
#include <Kernel/Logger.h>
#include <Kernel/Process/Loader/ELF.h>
#include <Kernel/Process/Loader/ELFDefs.h>
#include <Utils/Strings/Character.h>

namespace Kernel {
  using Utils::memcmp;

  ErrorOr<ELF::ElfHeader*> Loader::validate_elf_header(const Array<u8>& elf_buffer, ELF::FileType expected_type) {
    if(elf_buffer.size() < sizeof(ELF::ElfHeader)) {
      dbgln("Loader: ELF buffer too small to contain ELF header");
      return Error::create_from_string("ELF buffer too small to contain ELF header");
    }

    auto elf_header = reinterpret_cast<ELF::ElfHeader*>(elf_buffer.get_underlying_array());

    if(memcmp(elf_header->ident, ELFMAG, SELFMAG) != 0) {
      dbgln("Loader: Invalid ELF magic");
      return Error::create_from_string("Invalid ELF magic");
    }

    if(elf_header->ident[EI_CLASS] != ELFCLASS64) {
      dbgln("Loader: Unsupported ELF class: {}", elf_header->ident[EI_CLASS]);
      return Error::create_from_string("Unsupported ELF class");
    }

    if(elf_header->ident[EI_DATA] != ELFDATA2LSB) {
      dbgln("Loader: Unsupported ELF data encoding: {}", elf_header->ident[EI_DATA]);
      return Error::create_from_string("Unsupported ELF data encoding");
    }

    if(elf_header->machine != ELF::Machine::RISCV) {
      dbgln("Loader: Unsupported ELF machine type: {}", elf_header->machine);
      return Error::create_from_string("Unsupported ELF machine type");
    }

    if(elf_header->version != EV_CURRENT) {
      dbgln("Loader: Unsupported ELF version: {}", elf_header->version);
      return Error::create_from_string("Unsupported ELF version");
    }

    if(elf_header->type != expected_type) {
      if(expected_type == ELF::FileType::Executable && elf_header->type == ELF::FileType::Dynamic) {
        // Allow ET_DYN (shared library) when expecting ET_EXEC (executable)
      } else {
        dbgln("Loader: Unexpected ELF type: {}, expected: {}", elf_header->type, expected_type);
        return Error::create_from_string("Unexpected ELF type");
      }
    }

    if(elf_header->program_header_offset + (elf_header->program_header_entry_count * elf_header->program_header_entry_size) > elf_buffer.size()) {
      dbgln("Loader: ELF program headers exceed ELF buffer size");
      return Error::create_from_string("ELF program headers exceed ELF buffer size");
    }

    if(elf_header->section_header_offset + (elf_header->section_header_entry_count * elf_header->section_header_entry_size) > elf_buffer.size()) {
      dbgln("Loader: ELF section headers exceed ELF buffer size");
      return Error::create_from_string("ELF section headers exceed ELF buffer size");
    }

    return elf_header;
  }

  ErrorOr<void> Loader::parse_modinfo(const Array<u8>& elf_buffer, const ELF::SectionHeader* section_header, ArrayList<ModuleProperty>& out_properties) {
    char* modinfo_data = reinterpret_cast<char*>(elf_buffer.get_underlying_array() + section_header->offset);
    size_t modinfo_size = section_header->size;

    String current_string;
    ModuleProperty current_property;
    enum class ParseState {
      Key,
      Value
    } state = ParseState::Key;

    // Line format: "key=value\0"
    for(size_t i = 0; i < modinfo_size; i++) {
      char c = modinfo_data[i];
      if(state == ParseState::Key) {
        if(c == '=') {
          current_property.first = current_string;
          current_string = "";
          state = ParseState::Value;
        } else if(c == '\0') {
          return Error::create_from_string("Unexpected end of line. Expected '=' separator.");
        } else if(Character::is_printable(c)) {
          current_string += c;
        } else {
          return Error::create_from_string("Invalid character in modinfo key.");
        }
      } else {
        if(c == '\0') {
          current_property.second = current_string;
          out_properties.add(current_property);
          current_string = "";
          current_property = {};
          state = ParseState::Key;
        } else if(Character::is_printable(c)) {
          current_string += c;
        } else {
          return Error::create_from_string("Invalid character in modinfo value.");
        }
      }
    }

    if(state == ParseState::Value) {
      return Error::create_from_string("Unexpected end of modinfo. Expected null terminator after value.");
    }

    return {};
  }

  ErrorOr<void> Loader::load_module(Array<u8> module_in_buffer) {
    const auto elf_header = TRY(validate_elf_header(module_in_buffer, ELF::FileType::Relocatable));

    ELF::SectionHeader* shstrtab_section = TRY(get_section_header(module_in_buffer, elf_header, elf_header->section_header_string_table_index));
    char* section_names = reinterpret_cast<char*>(module_in_buffer.get_underlying_array() + shstrtab_section->offset);

    ArrayList<RefPtr<MemoryRegion>> allocated_regions;
    ArrayList<ModuleProperty> module_properties;
    ModuleHeader module_header {};


    for(size_t i = 0; i < elf_header->section_header_entry_count; i++) {
      ELF::SectionHeader* section_header = TRY(get_section_header(module_in_buffer, elf_header, i));
      auto section_name = String(&section_names[section_header->name_index]);

      if(section_name == ".modinfo") {
        TRY(parse_modinfo(module_in_buffer, section_header, module_properties));
      }

      if(section_name == ".module_header") {
        if(section_header->size != sizeof(ModuleHeader)) {
          dbgln("Loader: Invalid .module_header size: {}, expected: {}", section_header->size, sizeof(ModuleHeader));
          return Error::create_from_string("Invalid .module_header size");
        }
        module_header = *reinterpret_cast<ModuleHeader*>(module_in_buffer.get_underlying_array() + section_header->offset);
      }


      // Load only relevant sections
      if(section_header->type == ELF::SectionType::ProgBits ||
         section_header->type == ELF::SectionType::NoBits) {
        u64 section_size = section_header->size;
        u64 section_address = section_header->addr;
        u64 section_offset = section_header->offset;

        MemoryRegionPermissions permissions = MemoryRegionPermissions::Read;
        if(section_header->flags & (u64) ELF::SectionFlags::Write) {
          permissions = static_cast<MemoryRegionPermissions>(static_cast<u8>(permissions) | static_cast<u8>(MemoryRegionPermissions::Write));
        }
        if(section_header->flags & (u64) ELF::SectionFlags::ExecInstr) {
          permissions = static_cast<MemoryRegionPermissions>(static_cast<u8>(permissions) | static_cast<u8>(MemoryRegionPermissions::Execute));
        }

        // Allocate and map memory region
      }
    }
    // Parse .symtab and resolve symbols

    // Parse .module_header
    // Parse .modinfo

    // Relocate sections

    CPU::flush_instruction_cache();
    // Call init
  }
}// namespace Kernel