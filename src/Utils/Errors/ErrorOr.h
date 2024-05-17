/**
 * @file ErrorOr.h
 * @brief File containing Error and ErrorOr classes.
 *
 * @author Matěj Bucek
 */

#pragma once

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
    [[nodiscard]] StringView get_message() const { return m_message; }
    [[nodiscard]] int get_error() const { return m_code; }

    ~Error() = default;
  };

  template<typename T, typename E = Error>
  class ErrorOr {
  public:
    using ResultType = T;
    using ErrorType = E;

  protected:
    enum class ErrorOrType { Value,
                             Error,
                             Uninitialized } m_contains;
    union {
      ResultType m_value;
      ErrorType m_error;
    };

  protected:
    ErrorOr() : m_contains(ErrorOrType::Uninitialized) {}
    explicit ErrorOr(ResultType& value) : m_contains(ErrorOrType::Value), m_value(value) {}
    explicit ErrorOr(ResultType&& value) : m_contains(ErrorOrType::Value), m_value(move(value)) {}
    explicit ErrorOr(ErrorType error) : m_error(error) {}
    ErrorOr(const ErrorOr& other) {
      if(other.has_error()) {
        m_error = other.get_error();
      } else {
        m_value = other.get_value();
      }
    }

  public:
    static ErrorOr<T, E> create(ResultType& value) { return ErrorOr<T, E>(value); }
    static ErrorOr<T, E> create(ResultType&& value) { return ErrorOr<T, E>(move(value)); }
    static ErrorOr<T, E> create_error(ErrorType error) {
      return ErrorOr<T, E>(error);
    }
    ~ErrorOr() {
      if(m_contains == ErrorOrType::Value) {
        m_value.~ResultType();
      } else if(m_contains == ErrorOrType::Error) {
        m_error.~ErrorType();
      }
    }

    [[nodiscard]] bool has_value() const {
      return m_contains == ErrorOrType::Value;
    }
    [[nodiscard]] bool has_error() const {
      return m_contains == ErrorOrType::Error;
    }
    [[nodiscard]] ResultType get_value() const {
      return m_value;
    }
    [[nodiscard]] ErrorType get_error() const {
      return m_error;
    }
    [[nodiscard]] ResultType&& get_unique() {
      return move(m_value);
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
        m_error = other.get_error();
      } else {
        m_value = other.get_value();
      }
    }
  };

}// namespace Utils::Errors
