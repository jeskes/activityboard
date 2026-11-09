#pragma once

#include <button/button.h>
#include <modules/coreclient.h>
#include <numpad/numpad.h>

class InputController {
 public:
  InputController(ActivityCoreClient& core);
  void setup();

  Input get();
  void setWebInput(unsigned long timestamp, Input in );

 private:
  ActivityCoreClient& core;
  ActivityButton startButton;
  ActivityButton guessButton;
  ActivityButton jingleButton;
  ActivityNumpadClient numpad;

  Input webInput;
  unsigned long webInputTimestamp;
};