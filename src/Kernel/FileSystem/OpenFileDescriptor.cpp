#include <Kernel/FileSystem/OpenFileDescriptor.h>

namespace Kernel {

  void OpenFileDescriptor::open() {
    m_number_of_openers++;
  }

  bool OpenFileDescriptor::close() {
    return --m_number_of_openers == 0;
  }

  ErrorOr<RefPtr<OpenFileDescriptor>> OpenFileDescriptorTable::create_descriptor_and_open(const String& path, RefPtr<Inode> inode) {
    auto descriptor = RefPtr<OpenFileDescriptor>(new OpenFileDescriptor(inode));
    descriptor->open();
    m_descriptors.set(path, descriptor);
    return ErrorOr<RefPtr<OpenFileDescriptor>>::create(descriptor);
  }

  ErrorOr<RefPtr<OpenFileDescriptor>> OpenFileDescriptorTable::open(const String& path) {
    if(!m_descriptors.has_key(path)) {
      return ErrorOr<RefPtr<OpenFileDescriptor>>::create_error(Error::create_from_string("File not found"));
    }

    auto error_or_descriptor = m_descriptors.get(path);
    if(error_or_descriptor.has_error()) {
      return ErrorOr<RefPtr<OpenFileDescriptor>>::create_error(error_or_descriptor.get_error());
    }

    auto descriptor = error_or_descriptor.get_value();
    descriptor->open();
    return ErrorOr<RefPtr<OpenFileDescriptor>>::create(descriptor);
  }

  ErrorOr<void> OpenFileDescriptorTable::close(const String& path) {
    if(!m_descriptors.has_key(path)) {
      return ErrorOr<void>::create_error(Error::create_from_string("File not found"));
    }

    auto error_or_descriptor = m_descriptors.get(path);
    if(error_or_descriptor.has_error()) {
      return ErrorOr<void>::create_error(error_or_descriptor.get_error());
    }

    auto descriptor = error_or_descriptor.get_value();
    if(descriptor->close()) {
      m_descriptors.remove(path);
    }

    runtime_assert(descriptor->m_ref_count == 1, "OpenFileDescriptor still has references");

    return ErrorOr<void>::create({});
  }
}// namespace Kernel