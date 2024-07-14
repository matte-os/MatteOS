#pragma once

#include <Utils/Errors/ErrorOr.h>
#include <Utils/Pointers/RefCounted.h>
#include <Utils/Types.h>

namespace Kernel {
  using Utils::ErrorOr;

  class Inode : public RefCounted<Inode> {
  public:
    Inode() = default;
    virtual ~Inode() = default;

    virtual ErrorOr<size_t> read(u8* buffer, size_t size, size_t offset) = 0;
    virtual ErrorOr<size_t> write(const u8* buffer, size_t size, size_t offset) = 0;
  };
}// namespace Kernel
