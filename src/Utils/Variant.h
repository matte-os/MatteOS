//
// Created by matejbucek on 24.4.24.
//

#pragma once

namespace Utils {

template <typename T, typename... NextTypes> class Variant {
private:
  union {
    T m_value;
    Variant<NextTypes...> m_next;
  };
  bool m_isValue;

};

} // namespace Utils
