#include "rotaryencoder.h"

#ifdef ARDUINO_ARCH_ESP32

ActivityRotaryEncoder::ActivityRotaryEncoder(long initialValue, long minValue, long maxValue, unsigned long pressedTimeout)
    : ActivityButton(pressedTimeout),
      minValue(minValue),
      maxValue(maxValue),
      handler(nullptr),
      lastValue(initialValue),
      lastPosition(0) {
  ESP32Encoder::useInternalWeakPullResistors = puType::up;
}

void ActivityRotaryEncoder::setup(uint8_t clk, uint8_t dt, uint8_t sw) {
  ActivityButton::setup(sw);
  pinMode(clk, INPUT);
  pinMode(dt, INPUT);
  pinMode(sw, INPUT_PULLUP);
  encoder.attachHalfQuad(dt, clk);
  encoder.setCount(lastPosition);
}

void ActivityRotaryEncoder::loop() {
  if (handler) {
    long position = encoder.getCount();
    if (abs(position - lastPosition) > 1) {
      long value = (position > lastPosition) ? min(lastValue + 1, maxValue) : max(lastValue - 1, minValue);
      LOG("changed: old=%ld(%ld), new=%ld(%ld)", lastValue, lastPosition, value, position);
      if (lastValue != value) {
        lastValue = value;
        handler(value);
      }
      lastPosition = position;
    }
  }
}

void ActivityRotaryEncoder::reset() {
  encoder.setCount(0);
}

void ActivityRotaryEncoder::setHandler(std::function<void(long value)> handler) {
  this->handler = handler;
}

#endif