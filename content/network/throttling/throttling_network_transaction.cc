// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/network/throttling/throttling_network_transaction.h"

#include <utility>

#include "base/callback_helpers.h"
#include "content/network/throttling/throttling_controller.h"
#include "content/network/throttling/throttling_network_interceptor.h"
#include "content/network/throttling/throttling_upload_data_stream.h"
#include "net/base/load_timing_info.h"
#include "net/base/net_errors.h"
#include "net/http/http_network_transaction.h"
#include "net/http/http_request_info.h"
#include "net/socket/connection_attempts.h"

namespace content {

// Keep in sync with X_DevTools_Emulate_Network_Conditions_Client_Id defined in
// HTTPNames.json5.
const char
    ThrottlingNetworkTransaction::kDevToolsEmulateNetworkConditionsClientId[] =
        "X-DevTools-Emulate-Network-Conditions-Client-Id";

ThrottlingNetworkTransaction::ThrottlingNetworkTransaction(
    std::unique_ptr<net::HttpTransaction> network_transaction)
    : throttled_byte_count_(0),
      network_transaction_(std::move(network_transaction)),
      request_(nullptr),
      failed_(false) {}

ThrottlingNetworkTransaction::~ThrottlingNetworkTransaction() {
  if (interceptor_ && !throttle_callback_.is_null())
    interceptor_->StopThrottle(throttle_callback_);
}

void ThrottlingNetworkTransaction::IOCallback(
    const net::CompletionCallback& callback,
    bool start,
    int result) {
  result = Throttle(callback, start, result);
  if (result != net::ERR_IO_PENDING)
    callback.Run(result);
}

int ThrottlingNetworkTransaction::Throttle(
    const net::CompletionCallback& callback,
    bool start,
    int result) {
  if (failed_)
    return net::ERR_INTERNET_DISCONNECTED;
  if (!interceptor_ || result < 0)
    return result;

  base::TimeTicks send_end;
  if (start) {
    throttled_byte_count_ += network_transaction_->GetTotalReceivedBytes();
    net::LoadTimingInfo load_timing_info;
    if (GetLoadTimingInfo(&load_timing_info)) {
      send_end = load_timing_info.send_end;
      if (!load_timing_info.push_start.is_null())
        start = false;
    }
    if (send_end.is_null())
      send_end = base::TimeTicks::Now();
  }
  if (result > 0)
    throttled_byte_count_ += result;

  throttle_callback_ =
      base::Bind(&ThrottlingNetworkTransaction::ThrottleCallback,
                 base::Unretained(this), callback);
  int rv = interceptor_->StartThrottle(result, throttled_byte_count_, send_end,
                                       start, false, throttle_callback_);
  if (rv != net::ERR_IO_PENDING)
    throttle_callback_.Reset();
  if (rv == net::ERR_INTERNET_DISCONNECTED)
    Fail();
  return rv;
}

void ThrottlingNetworkTransaction::ThrottleCallback(
    const net::CompletionCallback& callback,
    int result,
    int64_t bytes) {
  DCHECK(!throttle_callback_.is_null());
  throttle_callback_.Reset();
  if (result == net::ERR_INTERNET_DISCONNECTED)
    Fail();
  throttled_byte_count_ = bytes;
  callback.Run(result);
}

void ThrottlingNetworkTransaction::Fail() {
  DCHECK(request_);
  DCHECK(!failed_);
  failed_ = true;
  network_transaction_->SetBeforeNetworkStartCallback(
      BeforeNetworkStartCallback());
  if (interceptor_)
    interceptor_.reset();
}

bool ThrottlingNetworkTransaction::CheckFailed() {
  if (failed_)
    return true;
  if (interceptor_ && interceptor_->IsOffline()) {
    Fail();
    return true;
  }
  return false;
}

int ThrottlingNetworkTransaction::Start(const net::HttpRequestInfo* request,
                                        const net::CompletionCallback& callback,
                                        const net::NetLogWithSource& net_log) {
  DCHECK(request);
  request_ = request;

  std::string client_id;
  bool has_devtools_client_id = request_->extra_headers.HasHeader(
      kDevToolsEmulateNetworkConditionsClientId);
  if (has_devtools_client_id) {
    custom_request_.reset(new net::HttpRequestInfo(*request_));
    custom_request_->extra_headers.GetHeader(
        kDevToolsEmulateNetworkConditionsClientId, &client_id);
    custom_request_->extra_headers.RemoveHeader(
        kDevToolsEmulateNetworkConditionsClientId);

    if (request_->upload_data_stream) {
      custom_upload_data_stream_.reset(
          new ThrottlingUploadDataStream(request_->upload_data_stream));
      custom_request_->upload_data_stream = custom_upload_data_stream_.get();
    }

    request_ = custom_request_.get();
  }

  ThrottlingNetworkInterceptor* interceptor =
      ThrottlingController::GetInterceptor(client_id);
  if (interceptor) {
    interceptor_ = interceptor->GetWeakPtr();
    if (custom_upload_data_stream_)
      custom_upload_data_stream_->SetInterceptor(interceptor);
  }

  if (CheckFailed())
    return net::ERR_INTERNET_DISCONNECTED;

  if (!interceptor_)
    return network_transaction_->Start(request_, callback, net_log);

  int result = network_transaction_->Start(
      request_,
      base::Bind(&ThrottlingNetworkTransaction::IOCallback,
                 base::Unretained(this), callback, true),
      net_log);
  return Throttle(callback, true, result);
}

int ThrottlingNetworkTransaction::RestartIgnoringLastError(
    const net::CompletionCallback& callback) {
  if (CheckFailed())
    return net::ERR_INTERNET_DISCONNECTED;
  if (!interceptor_)
    return network_transaction_->RestartIgnoringLastError(callback);

  int result = network_transaction_->RestartIgnoringLastError(
      base::Bind(&ThrottlingNetworkTransaction::IOCallback,
                 base::Unretained(this), callback, true));
  return Throttle(callback, true, result);
}

int ThrottlingNetworkTransaction::RestartWithCertificate(
    scoped_refptr<net::X509Certificate> client_cert,
    scoped_refptr<net::SSLPrivateKey> client_private_key,
    const net::CompletionCallback& callback) {
  if (CheckFailed())
    return net::ERR_INTERNET_DISCONNECTED;
  if (!interceptor_) {
    return network_transaction_->RestartWithCertificate(
        std::move(client_cert), std::move(client_private_key), callback);
  }

  int result = network_transaction_->RestartWithCertificate(
      std::move(client_cert), std::move(client_private_key),
      base::Bind(&ThrottlingNetworkTransaction::IOCallback,
                 base::Unretained(this), callback, true));
  return Throttle(callback, true, result);
}

int ThrottlingNetworkTransaction::RestartWithAuth(
    const net::AuthCredentials& credentials,
    const net::CompletionCallback& callback) {
  if (CheckFailed())
    return net::ERR_INTERNET_DISCONNECTED;
  if (!interceptor_)
    return network_transaction_->RestartWithAuth(credentials, callback);

  int result = network_transaction_->RestartWithAuth(
      credentials, base::Bind(&ThrottlingNetworkTransaction::IOCallback,
                              base::Unretained(this), callback, true));
  return Throttle(callback, true, result);
}

bool ThrottlingNetworkTransaction::IsReadyToRestartForAuth() {
  return network_transaction_->IsReadyToRestartForAuth();
}

int ThrottlingNetworkTransaction::Read(
    net::IOBuffer* buf,
    int buf_len,
    const net::CompletionCallback& callback) {
  if (CheckFailed())
    return net::ERR_INTERNET_DISCONNECTED;
  if (!interceptor_)
    return network_transaction_->Read(buf, buf_len, callback);

  int result = network_transaction_->Read(
      buf, buf_len,
      base::Bind(&ThrottlingNetworkTransaction::IOCallback,
                 base::Unretained(this), callback, false));
  // URLRequestJob relies on synchronous end-of-stream notification.
  if (result == 0)
    return result;
  return Throttle(callback, false, result);
}

void ThrottlingNetworkTransaction::StopCaching() {
  network_transaction_->StopCaching();
}

bool ThrottlingNetworkTransaction::GetFullRequestHeaders(
    net::HttpRequestHeaders* headers) const {
  return network_transaction_->GetFullRequestHeaders(headers);
}

int64_t ThrottlingNetworkTransaction::GetTotalReceivedBytes() const {
  return network_transaction_->GetTotalReceivedBytes();
}

int64_t ThrottlingNetworkTransaction::GetTotalSentBytes() const {
  return network_transaction_->GetTotalSentBytes();
}

void ThrottlingNetworkTransaction::DoneReading() {
  network_transaction_->DoneReading();
}

const net::HttpResponseInfo* ThrottlingNetworkTransaction::GetResponseInfo()
    const {
  return network_transaction_->GetResponseInfo();
}

net::LoadState ThrottlingNetworkTransaction::GetLoadState() const {
  return network_transaction_->GetLoadState();
}

void ThrottlingNetworkTransaction::SetQuicServerInfo(
    net::QuicServerInfo* quic_server_info) {
  network_transaction_->SetQuicServerInfo(quic_server_info);
}

bool ThrottlingNetworkTransaction::GetLoadTimingInfo(
    net::LoadTimingInfo* load_timing_info) const {
  return network_transaction_->GetLoadTimingInfo(load_timing_info);
}

bool ThrottlingNetworkTransaction::GetRemoteEndpoint(
    net::IPEndPoint* endpoint) const {
  return network_transaction_->GetRemoteEndpoint(endpoint);
}

void ThrottlingNetworkTransaction::PopulateNetErrorDetails(
    net::NetErrorDetails* details) const {
  return network_transaction_->PopulateNetErrorDetails(details);
}

void ThrottlingNetworkTransaction::SetPriority(net::RequestPriority priority) {
  network_transaction_->SetPriority(priority);
}

void ThrottlingNetworkTransaction::SetWebSocketHandshakeStreamCreateHelper(
    net::WebSocketHandshakeStreamBase::CreateHelper* create_helper) {
  network_transaction_->SetWebSocketHandshakeStreamCreateHelper(create_helper);
}

void ThrottlingNetworkTransaction::SetBeforeNetworkStartCallback(
    const BeforeNetworkStartCallback& callback) {
  network_transaction_->SetBeforeNetworkStartCallback(callback);
}

void ThrottlingNetworkTransaction::SetRequestHeadersCallback(
    net::RequestHeadersCallback callback) {
  network_transaction_->SetRequestHeadersCallback(std::move(callback));
}

void ThrottlingNetworkTransaction::SetResponseHeadersCallback(
    net::ResponseHeadersCallback callback) {
  network_transaction_->SetResponseHeadersCallback(std::move(callback));
}

void ThrottlingNetworkTransaction::SetBeforeHeadersSentCallback(
    const BeforeHeadersSentCallback& callback) {
  network_transaction_->SetBeforeHeadersSentCallback(callback);
}

int ThrottlingNetworkTransaction::ResumeNetworkStart() {
  if (CheckFailed())
    return net::ERR_INTERNET_DISCONNECTED;
  return network_transaction_->ResumeNetworkStart();
}

void ThrottlingNetworkTransaction::GetConnectionAttempts(
    net::ConnectionAttempts* out) const {
  network_transaction_->GetConnectionAttempts(out);
}

}  // namespace content