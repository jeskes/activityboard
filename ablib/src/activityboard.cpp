#include <Wire.h>

#include "ablib.h"

ActivityBoard activityBoard;

void ActivityBoard::setup() {
  Serial.begin(SERIAL_BAUD);
  while (!Serial) {
    delay(50);
  }
  LOG("ActivityBoard: initialize wire as gadget (master).");
  clearI2CBus();
#if defined(ARDUINO_ARCH_ESP32)
  if (!Wire.begin(WIRE_SDA, WIRE_SCL, 100000)) {
    LOG("ActivityBoard: failed to initialize wire.");
  }
  else {
    LOG("ActivityBoard: Wire initialized for gadget (master).");
  }
#else
  Wire.begin();
  // Wire.setTimeout(50);
  LOG("ActivityBoard: Wire initialized for gadget (master).");
#endif
}

void ActivityBoard::setup(uint8_t module) {
  LOG("ActivityBoard: initialize wire as peripherical service (module=%d).", module);
  Serial.begin(SERIAL_BAUD);
  while (!Serial) {
    delay(50);
  }
#if defined(ARDUINO_ARCH_ESP32)
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
  Wire.setTimeout(50);
#else
  Wire.begin(module);
  Wire.setTimeout(50);
  LOG("ActivityBoard: Wire initialized for peripherical slave %d.", module);
#endif
}

void ActivityBoard::clearI2CBus(uint8_t sda, uint8_t scl) {
  LOG("ActivityBoard: clear I2C bus at (%d,%d).", sda, scl);
  pinMode(sda, INPUT_PULLUP);
  pinMode(scl, OUTPUT);

  for (int i = 0; i < 9; i++) {
    digitalWrite(scl, LOW);
    delayMicroseconds(5);
    digitalWrite(scl, HIGH);
    delayMicroseconds(5);
    if (digitalRead(sda) == HIGH) {
      break;
    }
  }

  pinMode(sda, OUTPUT);
  digitalWrite(sda, LOW);  // SDA auf LOW
  delayMicroseconds(5);
  digitalWrite(scl, HIGH);  // SCL auf HIGH
  delayMicroseconds(5);
  digitalWrite(sda, HIGH);  // SDA von LOW auf HIGH ziehen (= I2C STOP)
  delayMicroseconds(5);
  pinMode(sda, INPUT_PULLUP);  // Wieder als Input freigeben

  LOG("ActivityBoard: cleared I2C bus at (%d,%d) with SDA=%d.", sda, scl, digitalRead(sda));
}
