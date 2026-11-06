#pragma once

#include <PCF8574.h>

#include <functional>

using ExtrasPinHandler = std::function<void()>;

class ExtrasExpansion {
 public:
  ExtrasExpansion();

  void setup();
  void loop();

  void showLed(uint8_t pin, bool on);
  void toggleLed(uint8_t pin);
  void addHandler(uint8_t pin, ExtrasPinHandler handler);

 private:
  PCF8574 pcf8574;
  ExtrasPinHandler handlers[8];
  unsigned long lastClickTime[8];
};
