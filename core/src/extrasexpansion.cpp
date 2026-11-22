#include "extrasexpansion.h"

#include <base/base.h>

#include "const.h"

static volatile bool extrasInterrupt = false;

static void IRAM_ATTR isrExtras() {
  extrasInterrupt = true;
}

ExtrasExpansion::ExtrasExpansion()
    : pcf8574(EXTRAS_EXPANSION_ID, &Wire1),
      shortPressHandlers({nullptr}),
      longPressHandlers({nullptr}),
      buttonPins(0),
      pressStartTime{0},
      longPressTriggered{false} {
}

void ExtrasExpansion::setup() {
  pcf8574.begin(0xFF);
  lastButtonState = 0xFF;

  pinMode(EXTRAS_EXPANSION_INT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(EXTRAS_EXPANSION_INT), isrExtras, FALLING);

  uint8_t state = pcf8574.read8(); /* initial read clears interrupt */
  LOG("extras extension initialized: connected=%d, state=0x%02X", pcf8574.isConnected(), state);
}

void ExtrasExpansion::loop() {
  unsigned long now = millis();
  uint8_t buttonState = lastButtonState;

  if (extrasInterrupt) {
    extrasInterrupt = false;
    buttonState = pcf8574.read8();
  }

  for (uint8_t pin = 0; pin < 8; pin++) {
    if (!(buttonPins & (1 << pin))) {
      continue; /* skip non-button pins */
    }

    bool isPressed = !(buttonState & (1 << pin));
    bool wasPressed = !(lastButtonState & (1 << pin));

    if (isPressed && !wasPressed) {
      if (now - pressStartTime[pin] > DEBOUNCE_DELAY) {
        pressStartTime[pin] = now;
        longPressTriggered[pin] = false;
      }
    }
    else if (isPressed && wasPressed) {
      if (longPressHandlers[pin] && !longPressTriggered[pin]) {
        if (now - pressStartTime[pin] >= LONGPRESS_DELAY) {
          longPressTriggered[pin] = true;
          longPressHandlers[pin]();
        }
      }
    }
    else if (!isPressed && wasPressed) {
      if (!longPressTriggered[pin] && shortPressHandlers[pin]) {
        if (now - pressStartTime[pin] >= DEBOUNCE_DELAY) {
          shortPressHandlers[pin]();
        }
      }
    }
  }

  lastButtonState = buttonState;
}

void ExtrasExpansion::showLed(uint8_t pin, bool on) {
  if (buttonPins & (1 << pin)) {
    LOG("cannot switch led on button pin");
    return;
  }
  // inverted : HIGH = off, LOW = on)
  pcf8574.write(pin, on ? LOW : HIGH);
}

void ExtrasExpansion::toggleLed(uint8_t pin) {
  if (buttonPins & (1 << pin)) {
    LOG("cannot switch led on button pin");
    return;
  }
  pcf8574.toggle(pin);
}

void ExtrasExpansion::initButtonPin(uint8_t pin) {
  buttonPins |= (1 << pin);
  pcf8574.write(pin, HIGH);
  clearInterrupt();
}

void ExtrasExpansion::addPressHandler(uint8_t pin, ExtrasPinHandler handler) {
  initButtonPin(pin);
  shortPressHandlers[pin] = handler;
}

void ExtrasExpansion::addLongPressHandler(uint8_t pin, ExtrasPinHandler handler) {
  initButtonPin(pin);
  longPressHandlers[pin] = handler;
}

void ExtrasExpansion::clearInterrupt() {
  pcf8574.read8(); /* clears interrupt */
}