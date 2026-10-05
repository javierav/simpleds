#pragma once

namespace simpleds::app {

// Composition root: builds the modules, wires their dependencies and drives
// their lifecycle. It must not contain domain logic.
class App {
 public:
  void begin();
  void update();
};

}  // namespace simpleds::app
