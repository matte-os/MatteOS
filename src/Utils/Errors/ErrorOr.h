/**
 * @file ErrorOr.h
 * @author Matěj Bucek (matejbucek)
 *
 * This file was inspired by SerenityOS.
 */
#pragma once

#include <Utils/Forwards.h>
#include <Utils/Strings/String.h>
#include <Utils/Traits.h>
#include <Utils/Variant.h>

#define TRY(expr)                           \
  ({                                        \
    auto _temporary_result = expr;          \
    if(_temporary_result.has_error()) {     \
      return _temporary_result.get_error(); \
    }                                       \
    _temporary_result.get_value();          \
  })

#define TRY_RETURN(expr)                    \
  ({                                        \
    auto _temporary_result = expr;          \
    if(_temporary_result.has_error()) {     \
      return _temporary_result.get_error(); \
    }                                       \
    return _temporary_result.get_value();   \
  })

namespace Utils {
  using namespace Utils;

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

    Error() = delete;

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

  template<typename T, typename E>
  class ErrorOr {
  public:
    using ResultType = T;
    using ErrorType = E;

  protected:
    Variant<ResultType, ErrorType> m_value_or_error;

    ErrorOr() = default;

  public:
    ErrorOr(ResultType& value) {
      m_value_or_error.template set<ResultType>(value);
    }

    ErrorOr(ResultType&& value) {
      m_value_or_error.template set<ResultType>(move(value));
    }

    ErrorOr(ErrorType error) {
      m_value_or_error.template set<ErrorType>(error);
    }

    ErrorOr(const ErrorOr& other) {
      if(other.has_error()) {
        m_value_or_error.template set<ErrorType>(other.get_error());
      } else {
        m_value_or_error.template set<ResultType>(other.get_value());
      }
    }

    static ErrorOr<T, E> create(T& value) { return ErrorOr<T, E>(value); }

    static ErrorOr<T, E> create(T&& value) { return ErrorOr<T, E>(move(value)); }

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

    operator bool() const {
      return has_value();
    }
  };

  class Empty {};

  template<typename E>
  class ErrorOr<void, E> : public ErrorOr<Empty, E> {
  public:
    using ResultType = Empty;
    using ErrorType = E;

    // Default constructor for success
    ErrorOr() {
      this->m_value_or_error.template set<ResultType>(Empty{});
    }

    // Constructor from ErrorType
    ErrorOr(ErrorType error) {
      this->m_value_or_error.template set<ErrorType>(error);
    }

    ErrorOr(const ErrorOr<Empty, E>& other)
        : ErrorOr<Empty, E>(other) {}// Properly initialize the base class
  };


}// namespace Utils
