#pragma once

namespace simpleds::services {

// Announces the device as <hostname>.local on the local network.
class MdnsResponder {
 public:
  // Starts the responder the first time it is called; later calls are no-ops.
  void begin(const char* hostname);

 private:
  bool started_ = false;
};

}  // namespace simpleds::services
