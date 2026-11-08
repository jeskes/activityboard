#pragma once

#include <button/button.h>
#include <numpad/numpad.h>

class InputController {
 public:
  void setup();

  Input get();

 private:
  ActivityButton startButton;
  ActivityButton repeatButton;
  ActivityButton nextButton;
  ActivityButton stopButton;
  ActivityNumpadClient numpad;
};