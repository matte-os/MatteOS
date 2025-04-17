/**
 * @file Function.h
 * @author Matěj Bucek (matejbucek)
 */
#pragma once

namespace Utils {
  template <typename ReturnType, typename... Args>
  using Function = auto (*)(Args...) -> ReturnType;
}