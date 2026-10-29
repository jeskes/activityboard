#pragma once

#include <button/button.h>
#include <numpad/numpad.h>

class InputController {
 public:
  void setup();

  bool selectPressed();
  bool startPressed();
  bool repeatPressed();
  bool skipPressed();
  bool cancelPressed();
  int numberPressed();

 private:
  ActivityButton selectButton;
  ActivityButton startButton;
  ActivityButton repeatButton;
  ActivityButton skipButton;
  ActivityButton cancelButton;
  ActivityNumpadClient numpad;
};