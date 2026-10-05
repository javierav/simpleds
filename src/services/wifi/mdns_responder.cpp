#include "services/wifi/mdns_responder.h"

#include <ESPmDNS.h>

namespace simpleds::services {

void MdnsResponder::begin(const char* hostname) {
  if (started_) return;
  started_ = MDNS.begin(hostname);
  if (started_) {
    MDNS.addService("http", "tcp", 80);
    log_i("mDNS responder started: %s.local", hostname);
  } else {
    log_e("Failed to start mDNS responder");
  }
}

}  // namespace simpleds::services
