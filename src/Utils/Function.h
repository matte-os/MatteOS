#pragma once

#include <Utils/Types.h>
#include <Utils/Pointers/RefPtr.h>
#include <Utils/Pointers/RefCounted.h>
#include <Utils/Utility.h>

namespace Utils {
  using Utils::Pointers::RefPtr;
  using Utils::Pointers::RefCounted;

  template <typename ReturnType, typename... Args>
  class Function {
  private:
    struct AnonymousFunction : public RefCounted<AnonymousFunction> {
      virtual ReturnType invoke(Args...) = 0;
      virtual ~AnonymousFunction() = default;
    };

    template <typename F>
    struct FunctionWrapper : public AnonymousFunction {
      F m_function;
      explicit FunctionWrapper(F function) : m_function(function) {}
      ReturnType invoke(Args... args) {
        return m_function(forward<Args...>(args...));
      }
    };
    RefPtr<AnonymousFunction> m_function;
  public:
    template <typename F>
    Function(F function) : m_function(new FunctionWrapper<F>(function)) {}
    ReturnType operator()(Args... args) {
      return m_function->invoke(forward<Args...>(args...));
    }
  };
}