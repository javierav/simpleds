#pragma once

#include <atomic>
#include <cstdint>
#include <string>

#include "network/connection_failure.h"
#include "network/wifi_credentials.h"

namespace simpleds::services {

// Tries to join a network with the given credentials while the access point
// stays up, and reports whether it worked.
class ConnectionTester {
 public:
  enum class State { kIdle, kTesting, kSucceeded, kFailed };

  // Registers the WiFi event handler. Call once.
  void begin();

  void start(const core::WifiCredentials& credentials);
  void update();

  State state() const { return state_; }
  core::ConnectionFailure failure() const { return failure_; }
  const core::WifiCredentials& credentials() const { return credentials_; }
  // IP address obtained during a successful test.
  const std::string& ip() const { return ip_; }

 private:
  void connect();
  void fail(core::ConnectionFailure failure);

  core::WifiCredentials credentials_;
  std::string ip_;
  State state_ = State::kIdle;
  core::ConnectionFailure failure_ = core::ConnectionFailure::kNone;
  uint32_t startedMs_ = 0;
  uint8_t attempts_ = 0;

  // Written from the WiFi event task, read from the main loop.
  std::atomic<bool> disconnected_{false};
  std::atomic<uint8_t> disconnectReason_{0};
};

}  // namespace simpleds::services
