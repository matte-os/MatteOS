/**
 * @file FileSystem.cpp
 * @author Matěj Bucek
 */
#include <Kernel/FileSystem/FileSystem.h>

Utils::ErrorOr<RefPtr<Kernel::OpenFileDescriptor>> Kernel::FileSystem::open(const Credentials& credentials, StringView path, FileOpenMode mode) {
  // Check if the file is already open in the open file table
  if(m_open_files.has_descriptor(path)) {
    return m_open_files.open(path);
  }

  // If the file is not open, open it and add it to the open file table
  //FIXME: StringView should have split too
  auto path_parts = String(path).split("/");

  auto root_inode = TRY(root());

  // If the path is empty, put the root inode into the open file table
  // and return the open file descriptor
  if(path_parts.size() == 0) {
    auto error_or_open_file_descriptor = m_open_files.create_descriptor_and_open(path, root_inode);
    if(error_or_open_file_descriptor.has_error()) {
      return ErrorOr<RefPtr<OpenFileDescriptor>>::create_error(error_or_open_file_descriptor.get_error());
    }

    return ErrorOr<RefPtr<OpenFileDescriptor>>::create(error_or_open_file_descriptor.get_value());
  }

  // Use the root inode to list the first directory entries
  auto inode = root_inode;
  size_t node_index = 0;
  String path_so_far = "";
  while(node_index < path_parts.size()) {
    // Check if the inode is a directory
    if(!inode->is_directory()) {
      return ErrorOr<RefPtr<OpenFileDescriptor>>::create_error(Error::create_from_string("Not a directory"));
    }

    auto& name = path_parts[node_index];
    auto directory_entries = TRY(inode->list_dir());

    auto contains_path_part = directory_entries.contains([name](auto entry) {
      return entry->get_name() == name;
    });

    if(!contains_path_part) {
      return ErrorOr<RefPtr<OpenFileDescriptor>>::create_error(Error::create_from_string("Path not found"));
    }

    path_so_far += "/";
    path_so_far += name;

    RefPtr<Inode> new_inode;

    if(m_open_files.has_descriptor(path_so_far)) {
      new_inode = TRY(m_open_files.open(path_so_far))->inode();
    } else {
      new_inode = TRY(inode->get_child(name));
      m_open_files.create_descriptor_and_open(path_so_far, new_inode);
    }

    if(root_inode != inode) {
      m_open_files.close(path_so_far);
    }

    inode = new_inode;
    node_index++;
  }

  return TRY(m_open_files.get_descriptor(path));
}