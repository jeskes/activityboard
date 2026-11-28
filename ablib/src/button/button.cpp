#include "button.h"

ActivityButton::ActivityButton(unsigned long pressedDelay, unsigned long longPressDelay, unsigned long doublePressDelay)
    : pressedDelay(pressedDelay),
      longPressDelay(longPressDelay),
      doublePressDelay(doublePressDelay),
      pressedPending(0),
      lastReleaseTime(0),
	  currentEvent(ButtonEvent::NONE),
      longPressHandled(false),
      doublePressHandled(false),
      waitingForDoublePress(false) {
}

void ActivityButton::setup(uint8_t pin) {
  this->pin = pin;
  pinMode(pin, INPUT_PULLUP);
}

ButtonEvent ActivityButton::update() {
  unsigned long now = millis();
  currentEvent = ButtonEvent::NONE;

  if (digitalRead(pin) == LOW) {
    if (pressedPending == 0) {
      pressedPending = now;
      longPressHandled = false;
      doublePressHandled = false;
    }
    else if (waitingForDoublePress && !doublePressHandled && (now - pressedPending >= pressedDelay)) {
      waitingForDoublePress = false;
      doublePressHandled = true;
	  currentEvent = ButtonEvent::DOUBLE_PRESS;
    }
    else if (!doublePressHandled && !longPressHandled && (now - pressedPending >= longPressDelay)) {
      longPressHandled = true;
	  currentEvent = ButtonEvent::LONG_PRESS;
    }
  }
  else {
    if (pressedPending != 0) {
      unsigned long pressDuration = now - pressedPending;
      pressedPending = 0;
      if (!longPressHandled && !doublePressHandled && (pressDuration >= pressedDelay)) {
        waitingForDoublePress = true;
        lastReleaseTime = now;
      }
    }
    else if (waitingForDoublePress && (now - lastReleaseTime >= doublePressDelay)) {
      waitingForDoublePress = false;
	  currentEvent = ButtonEvent::SHORT_PRESS;
    }
  }

  return currentEvent;
}