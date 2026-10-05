#include "app/app.h"

#include <Arduino.h>

#include "config/build_info.h"

namespace simpleds::app {

void App::begin() {
  // Outputs first, so the load and the fan are off as early as possible.
  ssr_.begin();
  fan_.begin();

  Serial.begin(115200);
  log_i("SimpleDS %s starting", config::kFirmwareVersion);

  ssrProbe_.begin();
  tankProbe_.begin();
  diverter_.begin(controlStore_.load());

  resetButton_.begin();
  wifi_.begin();
  if (wifi_.mode() == services::WifiManager::Mode::kStation) {
    mqtt_.begin(mqttStore_.load());
    panel_.begin();
  }
}

void App::update() {
  diverter_.update();
  if (resetButton_.update()) {
    // Regaining access to the device also unlocks the panel.
    passwordStore_.clear();
    wifi_.forgetNetwork();
  }
  wifi_.update();
  mqtt_.update(wifi_.isConnected());
  panel_.update();
  restart_.update();
}

}  // namespace simpleds::app
