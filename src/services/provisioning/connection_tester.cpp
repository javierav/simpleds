#include "services/provisioning/connection_tester.h"

#include <WiFi.h>

#include "config/network_defaults.h"

namespace simpleds::services {

void ConnectionTester::begin() {
  WiFi.onEvent(
      [this](arduino_event_id_t, arduino_event_info_t info) {
        disconnectReason_ = info.wifi_sta_disconnected.reason;
        disconnected_ = true;
      },
      ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
}

void ConnectionTester::start(const core::WifiCredentials& credentials) {
  credentials_ = credentials;
  state_ = State::kTesting;
  failure_ = core::ConnectionFailure::kNone;
  startedMs_ = millis();
  attempts_ = 0;
  ip_.clear();
  WiFi.setAutoReconnect(false);  // Retries are handled in update().
  connect();
}

void ConnectionTester::update() {
  if (state_ != State::kTesting) return;

  if (WiFi.status() == WL_CONNECTED) {
    state_ = State::kSucceeded;
    ip_ = WiFi.localIP().toString().c_str();
    log_i("Test connection to '%s' succeeded, IP %s", credentials_.ssid.c_str(), ip_.c_str());
    return;
  }

  if (disconnected_.exchange(false)) {
    const uint8_t reason = disconnectReason_;
    log_w("Test connection to '%s' failed, reason %u", credentials_.ssid.c_str(), reason);
    if (attempts_ >= config::kConnectionTestMaxAttempts) {
      fail(core::classifyDisconnectReason(reason));
    } else {
      connect();
    }
    return;
  }

  if (millis() - startedMs_ >= config::kConnectionTestTimeoutMs) {
    fail(core::ConnectionFailure::kTimeout);
  }
}

void ConnectionTester::connect() {
  ++attempts_;
  log_i("Testing connection to '%s' (attempt %u)", credentials_.ssid.c_str(), attempts_);
  disconnected_ = false;
  WiFi.begin(credentials_.ssid.c_str(), credentials_.password.c_str());
}

void ConnectionTester::fail(core::ConnectionFailure failure) {
  state_ = State::kFailed;
  failure_ = failure;
  WiFi.disconnect();  // Station only; the access point stays up.
  log_w("Connection test failed: %s", core::failureCode(failure));
}

}  // namespace simpleds::services
