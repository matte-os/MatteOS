/**
 * @file Assertions.h
 * @author Matěj Bucek (matejbucek)
 */
#pragma once

#ifdef __GNUC__
#define GET_CLASS_NAME (__PRETTY_FUNCTION__)
#else
#define GET_CLASS_NAME (__FUNCTION__)
#endif

#define runtime_assert(condition, message) runtime_assert_impl(condition, message, GET_CLASS_NAME, __LINE__)
void runtime_assert_impl(bool condition, const char* message, const char* file, int line);

#define use_after_free_assert(ptr) \
  if(Kernel::KernelMemoryAllocator::the().is_in_freed_memory(reinterpret_cast<uintptr_t*>(ptr))) { \
    __builtin_trap(); \
    runtime_assert(false, "Using freed memory"); \
  }