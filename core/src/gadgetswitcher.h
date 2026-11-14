#pragma once

#include <PCF8574.h>

/*
 * switch buttons : bits 0..3 (pressed=LOW, use pull-up)
 * gadget control : bits 4..7 (selected=HIGH)
 *
 * selected gadget : -1: none, 0..3 correspond to gadget 1..4
 */

class GadgetSwitcher {
 public:
  GadgetSwitcher();

  void setup();
  void loop();

  void allOff();
  void selectGadget(uint8_t index);
  int8_t getActiveGadget() const;

 private:
  PCF8574 pcf8574;
  int8_t activeGadget = -1;
  uint8_t lastButtonState = 0x0F;
  unsigned long lastDebounceTime = 0;

  static inline constexpr uint8_t BUTTONS_MASK = 0x0F;
  static inline constexpr uint8_t CONTROLS_MASK = 0xF0;
};
