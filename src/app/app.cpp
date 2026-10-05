#include "app/app.h"

#include <Arduino.h>

#include "config/build_info.h"

namespace simpleds::app {

void App::begin() {
  Serial.begin(115200);
  log_i("SimpleDS %s starting", config::kFirmwareVersion);
}

void App::update() {}

}  // namespace simpleds::app
