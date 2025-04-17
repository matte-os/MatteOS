/**
 * @file kmalloc.h
 * @author Matěj Bucek (matejbucek)
 */
#pragma once

#include <Kernel/Memory/KernelMemoryAllocator.h>

template<typename T>
T* kmalloc(size_t size) {
  return reinterpret_cast<T*>(Kernel::KernelMemoryAllocator::the().kmalloc(size));
}

template<typename T>
void kfree(T* ptr) {
  Kernel::KernelMemoryAllocator::the().kfree(reinterpret_cast<uintptr_t*>(ptr));
}