#include "base.h"

ActivityControls activityControls;

void ActivityControls::blink(uint8_t pin, uint16_t cycleCount, uint16_t timeout) {
  for (uint16_t cycle = 0; cycle < cycleCount; cycle++) {
    if (cycle > 0) {
      delay(timeout);
    }
    digitalWrite(pin, HIGH);
    delay(timeout);
    digitalWrite(pin, LOW);
  }
}

void ActivityControls::blink(const uint8_t pins[], uint8_t pinCount, uint16_t cycleCount, uint16_t timeout) {
  if (pinCount == 1) {
    blink(pins[0], cycleCount, timeout);
    return;
  }

  for (uint16_t cycle = 0; cycle < cycleCount; cycle++) {
    for (uint8_t idx = 0; idx < pinCount; idx++) {
      digitalWrite(pins[idx], LOW);
    }
    for (uint8_t idx = 0; idx < pinCount; idx++) {
      digitalWrite(pins[(idx - 1 + pinCount) % pinCount], LOW);
      digitalWrite(pins[idx], HIGH);
      delay(timeout);
    }
    for (uint8_t idx = 0; idx < pinCount; idx++) {
      digitalWrite(pins[idx], LOW);
    }
  }
}

bool ActivityControls::buttonPressed(uint8_t pin) {
  bool pressed = digitalRead(pin) == LOW;
  if (pressed) {
    delay(25);
    pressed = digitalRead(pin) == LOW;
    if (pressed) {
      while (digitalRead(pin) == LOW) {
      }
    }
  }
  return pressed;
}

void ActivityControls::buzzer(uint8_t buzzer, int frequency, long timeout) {
  tone(buzzer, frequency);
  delay(timeout);
  noTone(buzzer);
}

void ActivityControls::scanWire() {
  int nDevices = 0;
  LOG("scanning for I2C devices ...");
  for (byte address = 0x01; address < 0x7f; address++) {
    Wire.beginTransmission(address);
    uint8_t error = Wire.endTransmission();
    if (error == 0) {
      LOG("I2C device found at address 0x%02X", address);
      nDevices++;
    }
    else if (error != 2) {
      LOG("error %u at address 0x%02X", error, address);
    }
  }
  if (nDevices == 0) {
    LOG("no I2C devices found");
  }
}