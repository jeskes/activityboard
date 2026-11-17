#include "gadgetswitcher.h"

#include "const.h"

GadgetSwitcher::GadgetSwitcher()
    : pcf8574(SWITCH_EXPANSION_ID, &Wire1) {
}

void GadgetSwitcher::setup() {
  pcf8574.begin(BUTTONS_MASK);
  activeGadget = -1;
  LOG("gadget switcher initialized");
}

void GadgetSwitcher::loop() {
  uint8_t currentButtons = pcf8574.read8() & BUTTONS_MASK;

  if (currentButtons != lastButtonState) {
    lastDebounceTime = millis();
    lastButtonState = currentButtons;
  }

  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    for (uint8_t idx = 0; idx < 4; idx++) {
      if ((currentButtons & (1 << idx)) == 0) {
        if (activeGadget != idx) {
          selectGadget(idx);
          return;
        }
      }
    }
  }
}

void GadgetSwitcher::selectGadget(uint8_t index) {
  index = (index + 4) % 4;
  if (activeGadget == index) {
    return;
  }

  // break before make

  allOff();
  delay(15);
  pcf8574.write(index + 4, HIGH);
  activeGadget = index;

  LOG("switched gadgets %d", index + 1);
}

void GadgetSwitcher::allOff() {
  LOG("turn off all gadgets");
  for (uint8_t pin = 4; pin <= 7; pin++) {
    pcf8574.write(pin, LOW);
  }
  activeGadget = -1;
}

int8_t GadgetSwitcher::getActiveGadget() const {
  return activeGadget;
}
