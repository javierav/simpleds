#pragma once

#include <string>

#include "services/diverter/diverter_status.h"
#include "settings/control_settings.h"
#include "settings/mqtt_settings.h"
#include "settings/validation_error.h"

namespace simpleds::services {

// {"ssrTripC","ssrResetC","fanOnC","fanOffC","telemetryIntervalS","commandTimeoutS"}
std::string controlSettingsToJson(const core::ControlSettings& settings);

// {"host","port","username","hasPassword","clientId","commandTopic",…}; the
// password itself is never sent.
std::string mqttSettingsToJson(const core::MqttSettings& settings);

// {"state","reason","output","command","ssrTemperature","ssrSampled",
//  "tankTemperature","tankSampled","fan"}; unknown values are null.
std::string diverterStatusToJson(const DiverterStatus& status);

// {"error":"invalid","field","code"}
std::string validationErrorToJson(const core::ValidationError& error);

}  // namespace simpleds::services
