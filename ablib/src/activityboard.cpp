#include <Wire.h>

#include "ablib.h"

ActivityBoard activityBoard;

void ActivityBoard::init() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(WIRE_CLOCK);
  Wire.setTimeout(WIRE_TIMEOUT);
  LOG("Wire initialized.");
}

void ActivityBoard::init(int module) {
  Serial.begin(115200);
  Wire.begin(module);
  Wire.setClock(WIRE_CLOCK);
  Wire.setTimeout(WIRE_TIMEOUT);
  LOG("Wire initialized for peripherical  %d.", module);
}
