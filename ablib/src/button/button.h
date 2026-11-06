#pragma once

#include <Arduino.h>

#include "../base/base.h"

class ActivityButton {
 public:
  ActivityButton(unsigned long pressedTimeout = BUTTON_PRESSED_TIMEOUT);
  void setup(uint8_t pin);
  bool pressed();

 private:
  uint8_t pin;
  unsigned long pressedTimeout;
  unsigned long pressedPending;
};
