#include <Utils/Arrays/CircularStringBuffer.h>

namespace Utils {

  CircularStringBuffer::CircularStringBuffer(const size_t capacity)
      : m_size(capacity) {
    runtime_assert(capacity > 0, "CircularStringBuffer: capacity must be > 0");
    m_buffer = (char*) kmalloc<char>(capacity);
    memset(m_buffer, 0, m_size);
  }

  CircularStringBuffer::~CircularStringBuffer() {
    kfree(m_buffer);
  }

  void CircularStringBuffer::clear() {
    memset(m_buffer, 0, m_size);
    m_head = 0;
  }

  void CircularStringBuffer::write(const char* data, const size_t len) {
    // If writing would overflow, flush first
    if(m_head + len >= m_size) {
      flush();
    }

    memcpy(m_buffer + m_head, data, len);
    m_head += len;
  }

  void CircularStringBuffer::write(char c) {
    if(m_head + 1 >= m_size) {
      flush();
    }
    m_buffer[m_head++] = c;
  }

  void CircularStringBuffer::flush() {
    if(m_head == 0) return;
    m_buffer[m_head] = '\0';
    clear();
  }

  String CircularStringBuffer::to_string() const {
    return String(m_buffer, m_head);
  }

  bool CircularStringBuffer::is_empty() const {
    return m_head == 0;
  }

  size_t CircularStringBuffer::size() const {
    return m_head;
  }

  size_t CircularStringBuffer::capacity() const {
    return m_size;
  }
}// namespace Utils