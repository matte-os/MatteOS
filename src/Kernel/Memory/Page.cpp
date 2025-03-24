#include <Kernel/Memory/Page.h>

namespace Kernel {
  Page::Page() {
    m_status = PageStatus::Free;
  }

  Page::~Page() = default;

  bool Page::is_free() const {
    return m_status == PageStatus::Free;
  }

  bool Page::is_taken() const {
    return m_status == PageStatus::Taken;
  }

  bool Page::is_last() const {
    return m_status == PageStatus::Last;
  }

  void Page::clear() {
    m_status = PageStatus::Free;
  }

  void Page::set_status(const PageStatus status) {
    this->m_status = status;
  }
};// namespace Kernel
