/**
 * @file Math.h
 * @author Matěj Bucek (matejbucek)
 */
#pragma once

namespace Utils {
  template <typename T>
  T min(T first, T second) {
    return first < second ? first : second;
  }
}