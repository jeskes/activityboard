#include <Wire.h>

#include "ablib.h"

ActivityBoard activityBoard;

void ActivityBoard::setup() {
  Serial.begin(SERIAL_BAUD);
  while (!Serial) {
    delay(100);
  }
  delay(100);
  LOG("ActivityBoard: initialize wire as gadget (master).");
#if defined(ARDUINO_ARCH_ESP32)
  Wire.begin(WIRE_SDA, WIRE_SCL);
#else
  Wire.begin();
#endif
  LOG("Wire initialized for gadget (master).");
  pinMode(LED_BUILTIN, OUTPUT);
  activityControls.blink(LED_BUILTIN, 10, 100);
}

void ActivityBoard::setup(uint8_t module) {
  Serial.begin(SERIAL_BAUD);
  while (!Serial) {
    delay(50);
  }
  LOG("ActivityBoard: initialize wire as peripherical service (module=%d).", module);
#if defined(ARDUINO_ARCH_ESP32)
  Wire.end();
  delay(100);
  int trials = 40;
  bool success = false;
  while (trials && !(success = Wire.begin(module, WIRE_SDA, WIRE_SCL, 0))) {
    delay(50);
    trials--;
  }
  if (success) {
    LOG("ActivityBoard: Wire initialized for peripherical slave %d.", module);
  }
  else {
	LOG("ActivityBoard: Wire initialization failed.");
  }
#else
  Wire.begin(module);
  LOG("ActivityBoard: Wire initialized for peripherical slave %d.", module);
#endif

  pinMode(LED_BUILTIN, OUTPUT);
  activityControls.blink(LED_BUILTIN, 10, 100);
}
