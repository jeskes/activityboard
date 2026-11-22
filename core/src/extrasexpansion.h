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
  void addPressHandler(uint8_t pin, ExtrasPinHandler handler);
  void addLongPressHandler(uint8_t pin, ExtrasPinHandler handler);

 private:
  PCF8574 pcf8574;

  uint8_t buttonPins;
  void initButtonPin(uint8_t pin);
  ExtrasPinHandler shortPressHandlers[8];
  ExtrasPinHandler longPressHandlers[8];

  uint8_t lastButtonState;
  unsigned long pressStartTime[8];
  bool longPressTriggered[8];

  void clearInterrupt();
};
