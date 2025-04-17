/**
 * @file AsyncRequest.h
 * @author Matěj Bucek
 */
#pragma once

#include <Utils/Errors/ErrorOr.h>
#include <Utils/Types.h>

namespace Kernel {

  class AsyncResult : public RefCounted<AsyncResult> {
    s64 m_request_id;

  public:
    AsyncResult(s64 request_id) : m_request_id(request_id) {}

    s64 request_id() const { return m_request_id; }
  };

  class AsyncRequest : public RefCounted<AsyncRequest> {
    s64 m_id;

  public:
    AsyncRequest(s64 id) : m_id(id) {}

    s64 id() const { return m_id; }
  };

}// namespace Kernel
