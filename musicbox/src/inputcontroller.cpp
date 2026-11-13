#include "inputcontroller.h"

#include "const.h"

void InputController::setup() {
  startButton.setup(START_BUTTON);
  repeatButton.setup(REPEAT_BUTTON);
  nextButton.setup(NEXT_BUTTON);
  stopButton.setup(STOP_BUTTON);
  numpad.setup();
}

Input InputController::get() {
  if (startButton.pressed()) {
    return StdInput::START;
  }
  if (repeatButton.pressed()) {
    return StdInput::REPEAT;
  }
  if (nextButton.pressed()) {
    return StdInput::NEXT;
  }
  if (stopButton.pressed()) {
    return StdInput::STOP;
  }

  char ch = numpad.getChar();
  switch (ch) {
    case 'A':
      return StdInput::START;
    case 'B':
      return StdInput::REPEAT;
    case 'C':
      return StdInput::NEXT;
    case 'D':
      return StdInput::STOP;
    default:
      break;
  }
/*
  int num = ch - (int)'0';
  if (num >= 0 && num <= 9) {
    return (Input)num;
  }
*/
  return StdInput::NONE;
}
