#include <Kernel/Memory/Page.h>

namespace Kernel {
  Page::Page() {
    m_status = PageStatus::FREE;
  }

  Page::~Page() {
  }

  bool Page::is_free() {
    return m_status == PageStatus::FREE;
  }

  bool Page::is_taken() {
    return m_status == PageStatus::TAKEN;
  }

  bool Page::is_last() {
    return m_status == PageStatus::LAST;
  }

  void Page::clear() {
    m_status = PageStatus::FREE;
  }

  void Page::set_status(PageStatus status) {
    this->m_status = status;
  }
};// namespace Kernel
