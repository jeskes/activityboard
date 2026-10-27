#pragma once

#ifdef ARDUINO_ARCH_ESP32
#include <ESP32Encoder.h>

#include <functional>

#include "../button/button.h"

#define ROTARY_ENCODER_STEP 1

class ActivityRotaryEncoder : public ActivityButton {
 public:
  ActivityRotaryEncoder(long initialValue, long minValue, long maxValue, long step = ROTARY_ENCODER_STEP, unsigned long pressedTimeout = BUTTON_PRESSED_TIMEOUT);
  void setup(uint8_t clk, uint8_t dt, uint8_t sw);
  void loop();

  void reset();
  void setValue(long value);
  void setHandler(std::function<void(long value)> handler);

 private:
  long step;
  long minValue;
  long maxValue;
  long lastValue;
  long lastPosition;
  ESP32Encoder encoder;
  std::function<void(long value)> handler;
};
#endif
