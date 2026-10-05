#pragma once

#include <WebServer.h>

#include <array>
#include <cstddef>
#include <string>

#include "auth/login_throttle.h"
#include "services/storage/panel_password_store.h"

namespace simpleds::services {

// Optional password protection of the control panel. While no password is
// set the panel is open; once set, the API requires a session cookie obtained
// by logging in. Sessions live in RAM and end on restart or password change.
class PanelAuth {
 public:
  PanelAuth(WebServer& server, PanelPasswordStore& store);

  void begin();
  bool isProtected() const { return !password_.empty(); }

  // True if the current request may use the panel.
  bool isAuthorized();

  // POST /api/login (password) and POST /api/logout.
  void handleLogin();
  void handleLogout();

  // Stores a new password (empty: remove protection). Ends every session and,
  // if a password is set, starts a new one for the current client.
  bool changePassword(const std::string& password);

 private:
  static constexpr std::size_t kMaxSessions = 4;
  static constexpr std::size_t kTokenLength = 32;  // 128 random bits, hex.
  using Token = std::array<char, kTokenLength + 1>;

  void startSession();
  void endSessions();

  WebServer& server_;
  PanelPasswordStore& store_;
  std::string password_;
  std::array<Token, kMaxSessions> sessions_{};
  std::size_t nextSession_ = 0;
  core::LoginThrottle throttle_;
};

}  // namespace simpleds::services
