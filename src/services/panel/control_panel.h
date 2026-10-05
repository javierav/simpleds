#pragma once

#include <WebServer.h>

namespace simpleds::services {

// Web control panel (web/panel/) served on the local network in station mode.
class ControlPanel {
 public:
  void begin();
  void update();

 private:
  void handleGetStatus();
  void handleNotFound();

  WebServer server_{80};
  bool running_ = false;
};

}  // namespace simpleds::services
