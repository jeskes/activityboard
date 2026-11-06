#include "rotaryexpansion.h"

#include "const.h"

/* ========================================================================= */
/*                                                          rotary expansion */
/* ========================================================================= */

volatile bool rotaryInterrupt = false;

void IRAM_ATTR isrRotary() {
  rotaryInterrupt = true;
}

RotaryExpansion::RotaryExpansion()
    : decoder(ROTARY_EXPANSION_ID, &Wire1) {
}

void RotaryExpansion::setup() {
  attachInterrupt(digitalPinToInterrupt(ROTARY_EXPANSION_INT), isrRotary, FALLING);

  decoder.begin(2);
  decoder.setValue(0, 0);
  decoder.setClicks(0, 0);
  decoder.setStepsPerClick(0, 1);
  decoder.setValue(1, 0);
  decoder.setClicks(1, 0);
  decoder.setStepsPerClick(1, 1);
  delay(10);

  uint8_t initial = decoder.readInitialState();
  LOG("rotary expansion initialized: decoder connected=%d, state=0x%02X", decoder.isConnected(), initial);
}

void RotaryExpansion::loop() {
  if (rotaryInterrupt || digitalRead(ROTARY_EXPANSION_INT) == LOW) {
    rotaryInterrupt = false;
    LOG("interrupt rotary");

    if (decoder.update()) {
      if (adapterLeft) {
        adapterLeft->onInterrupt(*this, 0);
      }
      if (adapterRight) {
        adapterRight->onInterrupt(*this, 1);
      }
    }

    decoder.read8(); /* needed to clear int pin to HIGH */
  }
}

void RotaryExpansion::setAdapterLeft(RotaryAdapter& adapter) {
  adapterLeft = &adapter;
}

void RotaryExpansion::setAdapterRight(RotaryAdapter& adapter) {
  adapterRight = &adapter;
}

/* ========================================================================= */
/*                                                            rotary adapter */
/* ========================================================================= */

RotaryAdapter::RotaryAdapter(long initialValue, long minValue, long maxValue, long step, unsigned long pressedTimeout)
    : step(step),
      minValue(minValue),
      maxValue(maxValue),
      pressedTimeout(pressedTimeout),
      lastValue(initialValue),
      lastClicks(0) {
}

void RotaryAdapter::onPressed(std::function<void()> handler) {
  pressedHandler = handler;
}

void RotaryAdapter::onValueChanged(std::function<void(long value)> handler) {
  valueChangedHandler = handler;
}

void RotaryAdapter::onInterrupt(RotaryExpansion& rotary, int index) {
  int32_t clicks = rotary.decoder.getClicks(index);
  if (abs(clicks - lastClicks) > 3) {
    long value = (clicks - lastClicks > 0) ? min(lastValue + step, maxValue) : max(lastValue - step, minValue);
    // LOG("rotary %d change value: min=%ld, max=%ld, step=%ld, clicks: %ld ==> %ld, value: %ld ==> %ld", index, minValue, maxValue, step, lastClicks, clicks, lastValue, value);
    if (valueChangedHandler) {
      valueChangedHandler(value);
    }
    lastValue = value;
    lastClicks = clicks;
  }
  if (rotary.decoder.isKeyPressed(index)) {
    if (pressedHandler) {
      pressedHandler();
    }
    delay(pressedTimeout);
  }
}
