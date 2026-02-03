#pragma once

#include <Kernel/Memory/VirtualMemory/AddressSpace.h>
#include <Kernel/Process/Loader/ELF.h>
#include <Utils/Pair.h>

namespace Kernel {
  using Utils::Array;
  using Utils::Error;
  using Utils::ErrorOr;
  using Utils::String;

  using ModuleProperty = Utils::Pair<String, String>;

  class LoadedModule : public RefCounted<LoadedModule> {
    String m_name;
    String m_author;
    ArrayList<ModuleProperty> m_properties;
    AddressSpace m_address_space;
  };

  class LoadedExecutable {};

  class Loader {
    Loader() = default;

    ErrorOr<ELF::ElfHeader*> validate_elf_header(const Array<u8>& elf_buffer, ELF::FileType expected_type);
    ErrorOr<ELF::SectionHeader*> get_section_header(const Array<u8>& elf_buffer, const ELF::ElfHeader* elf_header, size_t index);
    ErrorOr<void> parse_modinfo(const Array<u8>& elf_buffer, const ELF::SectionHeader* section_header, ArrayList<ModuleProperty>& out_properties);

  public:
    static void init();
    static Loader& the();

    void load_executable();
    ErrorOr<void> load_module(Array<u8> module_in_buffer);
    void load_shared_library();
  };

}// namespace Kernel
