#include "inputcontroller.h"

#include "const.h"

void InputController::setup() {
  startButton.setup(START_BUTTON);
  repeatButton.setup(REPEAT_BUTTON);
  nextButton.setup(NEXT_BUTTON);
  cancelButton.setup(CANCEL_BUTTON);
  numpad.setup();
}

Input InputController::get() {
  if (startButton.pressed()) {
    return Input::START;
  }
  if (startButton.pressed()) {
    return Input::START;
  }
  if (repeatButton.pressed()) {
    return Input::REPEAT;
  }
  if (nextButton.pressed()) {
    return Input::NEXT;
  }
  if (cancelButton.pressed()) {
    return Input::CANCEL;
  }

  char ch = numpad.getChar();
  switch (ch) {
    case 'A':
      return Input::START;
    case 'B':
      return Input::REPEAT;
    case 'C':
      return Input::NEXT;
    case 'D':
      return Input::CANCEL;
    default:
      break;
  }

  int num = ch - (int)'0';
  if (num >= 0 && num <= 9) {
    return (Input)num;
  }

  return Input::NONE;
}
