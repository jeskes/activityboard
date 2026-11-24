#include "categoryswitch.h"

#include <Arduino.h>
#include <Wire.h>

#include "const.h"

static volatile bool categoryInterrupt = false;
static volatile unsigned long lastInterruptTime = 0;

static void IRAM_ATTR categoryIsr() {
  categoryInterrupt = true;
  lastInterruptTime = millis();
}

CategorySwitch::CategorySwitch()
    : pcf8574(CATEGORY_SWITCH_ID, &Wire) {
}

void CategorySwitch::setup() {
  pinMode(CATEGORY_SWITCH_IRQ, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(CATEGORY_SWITCH_IRQ), categoryIsr, FALLING);

  pcf8574.begin(0xFF);
  uint8_t state = pcf8574.read8(); /* initial read clears interrupt */

  LOG("initialized category switch: connected=%d, state=0x%02X, irq=%d", pcf8574.isConnected(), state, CATEGORY_SWITCH_IRQ);
}

MusicCategory CategorySwitch::loop() {
  if (categoryInterrupt && millis() > lastInterruptTime + DEBOUNCE_DELAY) {
    categoryInterrupt = false;
    return get();
  }
  return CATEGORY_UNDEF;
}

MusicCategory CategorySwitch::get() {
  Wire.requestFrom(CATEGORY_SWITCH_ID, (uint8_t)1);
  if (Wire.available()) {
    byte value = Wire.read();
    for (int idx = 0; idx < CATEGORY_COUNT; idx++) {
      if (bitRead(value, idx) == 0) {
        return idx;
      }
    }
  }
  return CATEGORY_UNDEF;
}