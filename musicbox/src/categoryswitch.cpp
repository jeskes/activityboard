#include "categoryswitch.h"

#include <Arduino.h>
#include <Wire.h>

#include "const.h"

CategorySwitch::CategorySwitch()
    : pcf8574(CATEGORY_SWITCH_ID, &Wire) {
}

void CategorySwitch::setup() {
  pcf8574.begin();

  uint8_t state = pcf8574.read8(); /* initial read */
  LOG("initialized extras extension: connected=%d, state=0x%02X", pcf8574.isConnected(), state);
}

uint8_t CategorySwitch::get() {
  Wire.requestFrom(CATEGORY_SWITCH_ID, (uint8_t)1);
  if (Wire.available()) {
    byte daten = Wire.read();
    for (int idx = 0; idx < CATEGORY_SWITCH_COUNT; idx++) {
      if (bitRead(daten, idx) == 0) {
        return idx + 1;
      }
    }
  }

  return 0;
}