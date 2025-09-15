/**
 * @file Request.h
 * @author Matěj Bucek
 */
#pragma once

#include <Utils/Optional.h>
#include <Utils/Pointers/RefCounted.h>
#include <Utils/Pointers/RefPtr.h>

namespace Kernel {
  using Utils::Optional;
  using Utils::RefPtr;
  using Utils::RefCounted;

  class RequestContext final : public RefCounted<RequestContext> {
    u64 m_pid;
    u64 m_tid;

    RequestContext() : m_pid(0), m_tid(0) {}

  public:
    static RefPtr<RequestContext> create_context() {
      return {new RequestContext};
    }

    void assign(u64 pid, u64 tid) {
      if(m_pid == 0) {
        m_pid = pid;
        m_tid = tid;
      }
    }

    u64 get_pid() {
      return m_pid;
    }

    u64 get_tid() {
      return m_tid;
    }
  };

  template<typename T>
  class Request final {
    Optional<T> m_value;
    RefPtr<RequestContext> m_context;

    enum class RequestState {
      Blocked,
      Finalized
    } m_state;

    explicit Request() : m_value(), m_context(RequestContext::create_context()), m_state(RequestState::Blocked) {}

    explicit Request(const RefPtr<RequestContext>& context) : m_value(), m_context(context), m_state(RequestState::Blocked) {}

    explicit Request(const T& value) : m_value(value), m_context(nullptr), m_state(RequestState::Finalized) {}
    explicit Request(T&& value) : m_value(move(value)), m_context(nullptr), m_state(RequestState::Finalized) {}

  public:
    static Request create_blocked() {
      return Request();
    }

    static Request create_finalized(T value) {
      return Request(value);
    }

    template<typename U>
   friend class Request;

    template<typename U>
    static Request propagate_blocked(const Request<U>& request) {
      return Request(request.m_context);
    }

    [[nodiscard]] bool is_blocked() const {
      return m_state == RequestState::Blocked;
    }

    [[nodiscard]] bool is_finalized() const {
      return m_state == RequestState::Finalized;
    }

    void assign(const u64 pid, const u64 tid) {
      if(m_context) {
        m_context->assign(pid, tid);
      }
    }

    RefPtr<RequestContext> get_context() const {
      return m_context;
    }

    T release_value() {
      runtime_assert(m_state == RequestState::Finalized, "The request is not finalized!");
      return m_value.value();
    }
  };
}// namespace Kernel
