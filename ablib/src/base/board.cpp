#include "base.h"

#ifdef ARDUINO_ARCH_ESP32
#include "driver/i2c.h"
/*
#include "hal/i2c_ll.h"
#include "soc/i2c_struct.h"
*/
#endif

ActivityBoard activityBoard;

/* ========================================================================= */
/*                                                                    master */
/* ========================================================================= */

void ActivityBoard::setup() {
  Serial.begin(SERIAL_BAUD);
  while (!Serial) {
    delay(50);
  }
  LOG("initialize wire as gadget (master).");
#if defined(ARDUINO_ARCH_ESP32)
  clearI2CBus(WIRE_SDA, WIRE_SCL);
  if (!Wire.begin(WIRE_SDA, WIRE_SCL, WIRE_SPEED)) {
    LOG("failed to initialize wire.");
  }
  else {
    LOG("Wire initialized for gadget (master).");
  }
#else
  clearI2CBus(WIRE_SDA, WIRE_SCL);
  Wire.begin();
  // Wire.setTimeout(50);
  LOG("Wire initialized for gadget (master).");
#endif
  Wire.setClock(WIRE_SPEED);
}

/* ========================================================================= */
/*                                                                     slave */
/* ========================================================================= */

void ActivityBoard::setup(uint8_t module) {
  Serial.begin(SERIAL_BAUD);
  while (!Serial) {
    delay(50);
  }
  LOG("initialize wire as peripherical service (module=%d).", module);
#if defined(ARDUINO_ARCH_ESP32)
  int trials = 40;
  bool success = false;
  LOG("initialize wire using sda=%d, scl=%d", WIRE_SDA, WIRE_SCL);
  while (trials && !(success = Wire.begin(module, WIRE_SDA, WIRE_SCL, WIRE_SPEED))) {
    delay(50);
    trials--;
  }
  if (success) {
    LOG("wire initialized for peripherical slave %d.", module);
  }
  else {
    LOG("wire initialization failed.");
  }
  i2c_set_timeout(I2C_NUM_0, 400000);
#else
  Wire.begin(module);
  Wire.setTimeout(50);
  LOG("wire initialized for peripherical slave %d.", module);
#endif
}

void ActivityBoard::clearI2CBus(uint8_t sda, uint8_t scl) {
  LOG("clear I2C bus at (%d,%d).", sda, scl);
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

  LOG("cleared I2C bus at (%d,%d) with levels SDA=%d, SCL=%d", sda, scl, digitalRead(sda), digitalRead(scl));
}
