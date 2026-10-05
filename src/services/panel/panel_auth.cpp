#include "services/panel/panel_auth.h"

#include <esp_random.h>

#include <cstdio>
#include <string_view>

#include "auth/cookie.h"
#include "auth/panel_password.h"
#include "services/web/web_responses.h"

namespace simpleds::services {

namespace {
constexpr const char* kCookieName = "sid";
constexpr uint8_t kMaxLoginFailures = 5;
constexpr uint32_t kLoginLockoutMs = 60000;
}  // namespace

PanelAuth::PanelAuth(WebServer& server, PanelPasswordStore& store)
    : server_(server), store_(store), throttle_(kMaxLoginFailures, kLoginLockoutMs) {}

void PanelAuth::begin() {
  password_ = store_.load();
  static const char* headers[] = {"Cookie"};
  server_.collectHeaders(headers, 1);
  if (isProtected()) log_i("Control panel protected by password");
}

bool PanelAuth::isAuthorized() {
  if (!isProtected()) return true;
  const String cookies = server_.header("Cookie");
  const std::string_view token = core::findCookie(cookies.c_str(), kCookieName);
  if (token.size() != kTokenLength) return false;
  for (const Token& session : sessions_) {
    if (core::constantTimeEquals(token, std::string_view(session.data(), kTokenLength))) {
      return true;
    }
  }
  return false;
}

void PanelAuth::handleLogin() {
  if (!isProtected()) {
    server_.send(204);
    return;
  }
  if (throttle_.isLocked(millis())) {
    sendJson(server_, 429, "{\"error\":\"locked\"}");
    return;
  }
  if (!core::constantTimeEquals(server_.arg("password").c_str(), password_)) {
    throttle_.recordFailure(millis());
    log_w("Failed login to the control panel");
    sendJson(server_, 401, "{\"error\":\"wrong_password\"}");
    return;
  }
  throttle_.recordSuccess();
  startSession();
  server_.send(204);
}

void PanelAuth::handleLogout() {
  // Only the client's own cookie is cleared; other sessions stay valid.
  server_.sendHeader("Set-Cookie", String(kCookieName) + "=; Path=/; Max-Age=0");
  server_.send(204);
}

bool PanelAuth::changePassword(const std::string& password) {
  if (!store_.save(password)) return false;
  password_ = password;
  endSessions();
  if (isProtected()) startSession();
  log_i("Control panel password %s", isProtected() ? "changed" : "removed");
  return true;
}

void PanelAuth::startSession() {
  Token& token = sessions_[nextSession_];
  nextSession_ = (nextSession_ + 1) % kMaxSessions;  // Replaces the oldest.
  for (std::size_t i = 0; i < kTokenLength / 8; ++i) {
    std::snprintf(&token[i * 8], 9, "%08x", static_cast<unsigned>(esp_random()));
  }
  server_.sendHeader("Set-Cookie", String(kCookieName) + "=" + token.data() +
                                       "; Path=/; HttpOnly; SameSite=Strict");
}

void PanelAuth::endSessions() {
  for (Token& session : sessions_) session.fill('\0');
}

}  // namespace simpleds::services
