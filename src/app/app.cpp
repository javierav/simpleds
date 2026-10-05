#include "app/app.h"

#include <Arduino.h>

#include "config/build_info.h"

namespace simpleds::app {

void App::begin() {
  Serial.begin(115200);
  log_i("SimpleDS %s starting", config::kFirmwareVersion);

  resetButton_.begin();
  wifi_.begin();
}

void App::update() {
  if (resetButton_.update()) wifi_.forgetNetwork();
  wifi_.update();
  restart_.update();
}

}  // namespace simpleds::app
