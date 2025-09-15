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
    alignas(ResultType) unsigned char m_result_data[sizeof(ResultType)];
    alignas(ErrorType) unsigned char m_error_data[sizeof(ErrorType)];
    bool m_has_value;

    // Helper methods to manage construction and destruction
    void construct_result_type(const ResultType& value) {
      new(&m_result_data) ResultType(value);// placement new
    }

    void construct_result_type(ResultType&& value) {
      new(&m_result_data) ResultType(move(value));// placement new
    }

    void construct_error_type(const ErrorType& error) {
      new(&m_error_data) ErrorType(error);// placement new
    }

    void construct_error_type(ErrorType&& error) {
      new(&m_error_data) ErrorType(move(error));// placement new
    }

    void destroy_result_type() {
      reinterpret_cast<ResultType*>(&m_result_data)->~ResultType();// manually call the destructor
    }

    void destroy_error_type() {
      reinterpret_cast<ErrorType*>(&m_error_data)->~ErrorType();// manually call the destructor
    }

  public:
    // Constructors for success and error
    ErrorOr(const ResultType& value) : m_has_value(true) {
      construct_result_type(value);
    }

    ErrorOr(ResultType&& value) : m_has_value(true) {
      construct_result_type(move(value));
    }

    ErrorOr(const ErrorType& error) : m_has_value(false) {
      construct_error_type(error);
    }

    ErrorOr(ErrorType&& error) : m_has_value(false) {
      construct_error_type(move(error));
    }

    // Copy constructor
    ErrorOr(const ErrorOr& other) : m_has_value(other.m_has_value) {
      if(m_has_value) {
        construct_result_type(other.get_value());
      } else {
        construct_error_type(other.get_error());
      }
    }

    // Destructor
    ~ErrorOr() {
      if(m_has_value) {
        destroy_result_type();
      } else {
        destroy_error_type();
      }
    }

    ErrorOr& operator=(const ErrorOr& other) {
      if(this == &other) {
        // First, destroy the current content
        if(m_has_value) {
          destroy_result_type();
        } else {
          destroy_error_type();
        }

        // Now copy from the other object
        m_has_value = other.m_has_value;
        if(m_has_value) {
          construct_result_type(other.get_value());
        } else {
          construct_error_type(other.get_error());
        }
      }
      return *this;
    }

    // Static creators
    static ErrorOr<T, E> create(const T& value) { return ErrorOr<T, E>(value); }

    static ErrorOr<T, E> create(T&& value) { return ErrorOr<T, E>(move(value)); }

    static ErrorOr<T, E> create_error(const E& error) { return ErrorOr<T, E>(error); }

    // Accessors
    bool has_value() const { return m_has_value; }

    bool has_error() const { return !m_has_value; }

    ResultType get_value() const {
      return *reinterpret_cast<const ResultType*>(&m_result_data);
    }

    ErrorType get_error() const {
      return *reinterpret_cast<const ErrorType*>(&m_error_data);
    }

    ResultType&& get_unique() {
      return move(*reinterpret_cast<ResultType*>(&m_result_data));
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
    ErrorOr() : ErrorOr<Empty, E>(Empty {}) {}

    // Constructor from ErrorType
    ErrorOr(ErrorType error) : ErrorOr<Empty, E>(move(error)) {}

    ErrorOr(const ErrorOr<Empty, E>& other)
        : ErrorOr<Empty, E>(other) {}// Properly initialize the base class
  };


}// namespace Utils
