#include "ablib.h"

ActivityControls activityControls;

void ActivityControls::blink(int pin, int cycleCount, int timeout) {
  for (int cycle = 0; cycle < cycleCount; cycle++) {
    if (cycle > 0) {
      delay(timeout);
    }
    digitalWrite(pin, HIGH);
    delay(timeout);
    digitalWrite(pin, LOW);
  }
}

void ActivityControls::blink(const int pins[], int pinCount, int cycleCount,
                             int timeout) {
  if (pinCount == 1) {
    blink(pins[0], cycleCount, timeout);
    return;
  }

  for (int cycle = 0; cycle < cycleCount; cycle++) {
    for (int idx = 0; idx < pinCount; idx++) {
      digitalWrite(pins[idx], LOW);
    }
    for (int idx = 0; idx < pinCount; idx++) {
      digitalWrite(pins[(idx - 1 + pinCount) % pinCount], LOW);
      digitalWrite(pins[idx], HIGH);
      delay(timeout);
    }
    for (int idx = 0; idx < pinCount; idx++) {
      digitalWrite(pins[idx], LOW);
    }
  }
}

bool ActivityControls::buttonPressed(int pin) {
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
