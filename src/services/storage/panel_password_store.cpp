#include "services/storage/panel_password_store.h"

#include <Preferences.h>

namespace simpleds::services {

namespace {
constexpr const char* kNamespace = "panel";
constexpr const char* kPasswordKey = "password";
}  // namespace

std::string PanelPasswordStore::load() {
  Preferences prefs;
  if (!prefs.begin(kNamespace, false)) return "";
  const std::string password = prefs.getString(kPasswordKey, "").c_str();
  prefs.end();
  return password;
}

bool PanelPasswordStore::save(const std::string& password) {
  if (password.empty()) {
    clear();
    return true;
  }
  Preferences prefs;
  if (!prefs.begin(kNamespace, false)) return false;
  const bool ok = prefs.putString(kPasswordKey, password.c_str()) == password.size();
  prefs.end();
  return ok;
}

void PanelPasswordStore::clear() {
  Preferences prefs;
  if (!prefs.begin(kNamespace, false)) return;
  prefs.clear();
  prefs.end();
}

}  // namespace simpleds::services
