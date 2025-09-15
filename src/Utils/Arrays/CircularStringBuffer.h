//
// Created by matejbucek on 12.09.25.
//

#pragma once

#include <Utils/Assertions.h>
#include <Utils/Memory.h>
#include <Utils/Strings/String.h>
#include <Utils/Types.h>
#include <Utils/kmalloc.h>

namespace Utils {

  class CircularStringBuffer {
    char* m_buffer = {nullptr};
    size_t m_size = 0;
    size_t m_head = 0;

  public:
    explicit CircularStringBuffer(size_t capacity);

    ~CircularStringBuffer();

    void clear();

    void write(const char* data, const size_t len);

    void write(char c);

    void flush();

    String to_string() const;

    bool is_empty() const;

    size_t size() const;

    size_t capacity() const;
  };

}// namespace Utils
