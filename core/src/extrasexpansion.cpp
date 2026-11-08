#include "extrasexpansion.h"

#include <base/base.h>

#include "const.h"

volatile bool extrasInterrupt = false;

void IRAM_ATTR isrExtras() {
  extrasInterrupt = true;
}

ExtrasExpansion::ExtrasExpansion()
    : pcf8574(EXTRAS_EXPANSION_ID, &Wire1),
      handlers({0}) {
}

void ExtrasExpansion::setup() {
  pcf8574.begin();

  pinMode(EXTRAS_EXPANSION_INT, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(EXTRAS_EXPANSION_INT), isrExtras, FALLING);
  
  uint8_t state = pcf8574.read8(); /* initial read */
  LOG("initialized extras extension: connected=%d, state=0x%02X", pcf8574.isConnected(), state);
}

void ExtrasExpansion::loop() {
  if (extrasInterrupt || digitalRead(EXTRAS_EXPANSION_INT) == LOW) {
    extrasInterrupt = false;
    LOG("extras interrupt");

    uint8_t reg = pcf8574.read8();

    for (uint8_t pin = 0; pin < 8; pin++) {
      if (!(reg & (1 << pin))) {
        // debounce
        if (millis() - lastClickTime[pin] > BUTTON_PRESSED_TIMEOUT) {
          lastClickTime[pin] = millis();
          if (handlers[pin] != nullptr) {
            handlers[pin]();
          }
        }
      }
    }
    pcf8574.read8(); /* clear interrupt */
  }
}

void ExtrasExpansion::showLed(uint8_t pin, bool on) {
  // inverted : HIGH = off, LOW = on)
  pcf8574.write(pin, on ? LOW : HIGH);
}

void ExtrasExpansion::toggleLed(uint8_t pin) {
  uint8_t state = pcf8574.read(pin);
  pcf8574.write(pin, state == HIGH ? LOW : HIGH);
}

void ExtrasExpansion::addHandler(uint8_t pin, ExtrasPinHandler handler) {
  // configure for internal pull-up
  pcf8574.write(pin, HIGH);
  handlers[pin] = handler;
}
