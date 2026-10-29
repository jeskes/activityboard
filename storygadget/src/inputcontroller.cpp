#include "inputcontroller.h"

#include "const.h"

void InputController::setup() {
  selectButton.setup(SELECT_BUTTON);
  startButton.setup(START_BUTTON);
  repeatButton.setup(REPEAT_BUTTON);
  skipButton.setup(SKIP_BUTTON);
  cancelButton.setup(CANCEL_BUTTON);
  numpad.setup();
}

Input InputController::get() {
  if (selectButton.pressed()) {
    return Input::SELECT;
  }
  if (startButton.pressed()) {
    return Input::START;
  }
  if (startButton.pressed()) {
    return Input::START;
  }
  if (repeatButton.pressed()) {
    return Input::REPEAT;
  }
  if (skipButton.pressed()) {
    return Input::SKIP;
  }
  if (cancelButton.pressed()) {
    return Input::CANCEL;
  }

  char ch = numpad.getChar();
  switch (ch) {
    case 'A':
      return Input::REPEAT;
    case 'B':
      return Input::SKIP;
    case 'C':
      return Input::CANCEL;
    case 'D':
      return Input::START;
    case '*':
      return Input::SELECT;
    default:
      break;
  }

  int num = ch - (int)'0';
  if (num >= 0 && num <= 9) {
    return (Input)num;
  }

  return Input::NONE;
}
