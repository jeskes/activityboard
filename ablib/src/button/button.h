#pragma once

#include <Arduino.h>

#include "../base/base.h"

enum class ButtonEvent {
  NONE,
  SHORT_PRESS,
  DOUBLE_PRESS,
  LONG_PRESS
};

class ActivityButton {
 public:
  ActivityButton(unsigned long pressedDelay = DEBOUNCE_DELAY, unsigned long longPressDelay = LONGPRESS_DELAY, unsigned long doublePressDelay = DOUBLEPRESS_DELAY);
  void setup(uint8_t pin);

  ButtonEvent update();

  inline bool pressed() {
    return currentEvent == ButtonEvent::SHORT_PRESS;
  }
  inline bool longPress() {
    return currentEvent == ButtonEvent::LONG_PRESS;
  }
  inline bool doubleClick() {
    return currentEvent == ButtonEvent::DOUBLE_PRESS;
  }

 private:
  uint8_t pin;
  unsigned long pressedDelay;
  unsigned long longPressDelay;
  unsigned long doublePressDelay;
  unsigned long pressedPending;
  unsigned long lastReleaseTime;
  ButtonEvent currentEvent;
  bool longPressHandled;
  bool doublePressHandled;
  bool waitingForDoublePress;
};
