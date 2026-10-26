#include "button.h"

ActivityButton::ActivityButton(unsigned long pressedTimeout)
    : pressedTimeout(pressedTimeout),
      pressedPending(0) {
}

void ActivityButton::setup(uint8_t pin) {
  this->pin = pin;
  pinMode(pin, INPUT_PULLUP);
}

bool ActivityButton::pressed() {
  unsigned long now = millis();
  if (digitalRead(pin) == LOW) {
    if (!pressedPending) {
      pressedPending = now;
    }
  }
  else {
    if (pressedPending && (now - pressedPending > pressedTimeout)) {
      pressedPending = 0;
      return true;
    }
	else {
		pressedPending = 0;
	}
  }
  return false;
}
