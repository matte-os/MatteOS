#pragma once

#include <Utils/Errors/ErrorOr.h>

namespace Utils {

  template <typename Key, typename Value>
  class Map {
  public:
    Map() = default;
    virtual ~Map() = default;

    virtual ErrorOr<Value> get(const Key& key) = 0;
    virtual void set(const Key& key, const Value& value) = 0;
    virtual void remove(const Key& key) = 0;
  };

}// namespace Utils
