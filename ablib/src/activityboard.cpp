#include <Wire.h>

#include "ablib.h"

ActivityBoard activityBoard;

void ActivityBoard::init() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(WIRE_CLOCK);
  Wire.setTimeout(WIRE_TIMEOUT);
  LOG("Wire initialized for gadget.");
  pinMode(LED_BUILTIN, OUTPUT);
  activityControls.blink(LED_BUILTIN, 10, 100);
}

void ActivityBoard::init(int module) {
  Serial.begin(115200);
  Wire.begin(module);
  Wire.setClock(WIRE_CLOCK);
  Wire.setTimeout(WIRE_TIMEOUT);
  LOG("Wire initialized for peripherical  %d.", module);
  pinMode(LED_BUILTIN, OUTPUT);
  activityControls.blink(LED_BUILTIN, 10, 100);
}
