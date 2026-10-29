#pragma once

#include <button/button.h>
#include <numpad/numpad.h>

enum class Input : int {
  NONE = -1,
  NUM_0 = 0,
  NUM_1 = 1,
  NUM_2 = 2,
  NUM_3 = 3,
  NUM_4 = 4,
  NUM_5 = 5,
  NUM_6 = 6,
  NUM_7 = 7,
  NUM_8 = 8,
  NUM_9 = 9,
  SELECT = 10,
  START = 11,
  REPEAT = 12,
  SKIP = 13,
  CANCEL = 14
};

class InputController {
 public:
  void setup();

  Input get();

 private:
  ActivityButton selectButton;
  ActivityButton startButton;
  ActivityButton repeatButton;
  ActivityButton skipButton;
  ActivityButton cancelButton;
  ActivityNumpadClient numpad;
};