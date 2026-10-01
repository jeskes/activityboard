#include <Wire.h>

#include "ablib.h"

ActivityBoard activityBoard;

void ActivityBoard::setup() {
  Serial.begin(115200);
#if defined(ARDUINO_ARCH_ESP32)
  Wire.begin(WIRE_SDA, WIRE_SCL);
  Wire.setClock(WIRE_CLOCK);
#else
  Wire.begin();
  Wire.setClock(WIRE_CLOCK);
  Wire.setTimeout(WIRE_TIMEOUT);
#endif
  LOG("Wire initialized for gadget.");
  pinMode(LED_BUILTIN, OUTPUT);
  activityControls.blink(LED_BUILTIN, 10, 100);
}

void ActivityBoard::setup(uint8_t module) {
  Serial.begin(115200);
#if defined(ARDUINO_ARCH_ESP32)
  Wire.begin(WIRE_SDA, WIRE_SCL);
  Wire.setClock(WIRE_CLOCK);
#else
  Wire.begin(module);
  Wire.setClock(WIRE_CLOCK);
  Wire.setTimeout(WIRE_TIMEOUT);
#endif
  LOG("Wire initialized for peripherical %d.", module);
  pinMode(LED_BUILTIN, OUTPUT);
  activityControls.blink(LED_BUILTIN, 10, 100);
}
