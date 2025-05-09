/**
 * @file HashFunctions.cpp
 * @author Matěj Bucek (matejbucek)
 */
#pragma once

#include <Utils/Function.h>
#include <Utils/Strings/String.h>
#include <Utils/Types.h>

namespace Utils {
  struct HashInput {
    const char* data;
    size_t size;
  };

  HashInput to_hash_input(u64 value);
  HashInput to_hash_input(const char* value);
  HashInput to_hash_input(const String& value);
  HashInput to_hash_input(const StringView& value);

  template<typename T>
  concept Hashable = requires(T t) {
    { to_hash_input(t) } -> SameAs<HashInput>;
  };

  template<Hashable Input, typename Output>
  using HashFunction = Function<Output, Input>;

  namespace HashFunctions {
    template <Hashable Input, typename Output>
    Output djb2(Input value) {
      auto input = to_hash_input(value);
      Output hash = 5381;
      for(size_t i = 0; i < input.size; i++) {
        hash = (hash << 5) + hash + input.data[i];
      }
      return hash;
    }
  }// namespace HashFunctions

}// namespace Utils