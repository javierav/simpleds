// Arduino entry point. Keep this file thin: all logic lives in app::App.

#include <Arduino.h>

#include "app/app.h"

namespace {
simpleds::app::App app;
}  // namespace

void setup() { app.begin(); }

void loop() { app.update(); }
