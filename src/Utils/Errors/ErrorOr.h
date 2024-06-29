/**
 * @file ErrorOr.h
 * @brief File containing Error and ErrorOr classes.
 *
 * @author Matěj Bucek
 */

#pragma once

#include "Utils/Variant.h"
#include <Utils/Strings/String.h>
#include <Utils/Utility.h>

namespace Utils::Errors {
  using namespace Utils::Strings;

#define PROPAGATE_IF_ERROR(maybe_error) return maybe_error;

  class Error {
  protected:
    String m_message;
    int m_code;

  private:
    explicit Error(int code) : m_code(code) {}
    explicit Error(String&& message)
        : m_message(move(message)), m_code(1) {}
    explicit Error(String&& message, int code)
        : m_message(move(message)), m_code(code) {}

  public:
    Error(const Error& other) = default;
    static Error create_from_string(String&& message) {
      return Error(move(message));
    }
    static Error create_from_errno(int error) { return Error(error); }
    static Error create(String&& message, int error) {
      return Error(move(message), error);
    }
    [[nodiscard]] bool has_error() const { return m_code; }
    [[nodiscard]] StringView get_message() const { return static_cast<StringView>(m_message); }
    [[nodiscard]] int get_error() const { return m_code; }

    ~Error() = default;
  };

  template<typename T, typename E = Error>
  class ErrorOr {
  public:
    using ResultType = T;
    using ErrorType = E;

  protected:
    Variant<ResultType, ErrorType> m_value_or_error;

  protected:
    ErrorOr() = default;
    explicit ErrorOr(ResultType&& value) {
      m_value_or_error.template set<ResultType>(move(value));
    }
    explicit ErrorOr(ErrorType error) {
      m_value_or_error.template set<ErrorType>(error);
    }

    ErrorOr(const ErrorOr& other) {
      if(other.has_error()) {
        m_value_or_error.template set<ErrorType>(other.get_error());
      } else {
        m_value_or_error.template set<ResultType>(other.get_value());
      }
    }

  public:
    static ErrorOr<T, E> create(ResultType&& value) { return ErrorOr<T, E>(move(value)); }
    static ErrorOr<T, E> create_error(ErrorType error) {
      return ErrorOr<T, E>(error);
    }
    ~ErrorOr() = default;

    [[nodiscard]] bool has_value() const {
      return m_value_or_error.template is<ResultType>();
    }
    [[nodiscard]] bool has_error() const {
      return m_value_or_error.template is<ErrorType>();
    }
    [[nodiscard]] ResultType get_value() const {
      return m_value_or_error.template as<ResultType>();
    }
    [[nodiscard]] ErrorType get_error() const {
      return m_value_or_error.template as<ErrorType>();
    }
    [[nodiscard]] ResultType&& get_unique() {
      return move(m_value_or_error.template as<ResultType>());
    }
  };

  class Empty {};

  template<>
  class ErrorOr<void> : public ErrorOr<Empty> {
  public:
    using ResultType = Empty;
    using ErrorType = Error;

    explicit ErrorOr(const ErrorOr<Empty, Error>& other) : ErrorOr<Empty>() {
      if(other.has_error()) {
        m_value_or_error.template set<ErrorType>(other.get_error());
      } else {
        m_value_or_error.template set<ResultType>(other.get_value());
      }
    }
  };

}// namespace Utils::Errors
