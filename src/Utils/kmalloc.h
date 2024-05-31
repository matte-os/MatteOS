//
// Created by matejbucek on 10.9.22.
//

#pragma once

#include <Kernel/Memory/KernelMemoryAllocator.h>

template<typename T>
T* kmalloc(size_t size) {
  return reinterpret_cast<T*>(Kernel::Memory::KernelMemoryAllocator::the().kmalloc(size));
}

template<typename T>
void kfree(T* ptr) {
  Kernel::Memory::KernelMemoryAllocator::the().kfree(reinterpret_cast<uintptr_t*>(ptr));
}