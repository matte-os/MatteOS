#pragma once

#include <Utils/Pointers/RefCounted.h>
#include <Utils/Pointers/RefPtr.h>
#include <Utils/Traits.h>
#include <Utils/Types.h>

namespace Utils {
  using Utils::RefCounted;
  using Utils::RefPtr;

  template<typename ReturnType, typename... Args>
  class Callable {
  private:
    struct AnonymousFunction : public RefCounted<AnonymousFunction> {
      virtual ReturnType invoke(Args...) = 0;
      virtual ~AnonymousFunction() = default;
    };

    template<typename F>
    struct FunctionWrapper : public AnonymousFunction {
      F m_function;
      explicit FunctionWrapper(F function) : m_function(function) {}
      ReturnType invoke(Args... args) {
        return m_function(forward<Args...>(args...));
      }
    };
    RefPtr<AnonymousFunction> m_function;

  public:
    template<typename F>
    Callable(F function) : m_function(new FunctionWrapper<F>(function)) {}
    ReturnType operator()(Args... args) {
      return m_function->invoke(forward<Args...>(args...));
    }
  };
}// namespace Utils