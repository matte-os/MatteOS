//
// Created by matejbucek on 10.5.24.
//

#pragma once

#include <Utils/Pointers/RefCounted.h>
#include <Utils/Arrays/Array.h>


namespace Utils {
  template <typename T>
  class SharedArray : public Array<T>, public RefCounted<SharedArray<T>> {};
};
