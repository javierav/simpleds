#pragma once

#include <WebServer.h>

#include "settings/control_settings.h"
#include "settings/mqtt_settings.h"

namespace simpleds::services {

// Reads the settings posted by the panel (form fields named as in the JSON).
// Malformed numbers become values that the validation rejects.
core::ControlSettings controlSettingsFromForm(WebServer& server);

// An empty password field keeps the current password, unless the username is
// also empty (anonymous access).
core::MqttSettings mqttSettingsFromForm(WebServer& server, const core::MqttSettings& current);

}  // namespace simpleds::services
