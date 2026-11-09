#include "inputcontroller.h"

#include "const.h"

InputController::InputController(ActivityCoreClient& core)
    : core(core) {
}

void InputController::setup() {
  startButton.setup(START_BUTTON);
  guessButton.setup(GUESS_BUTTON);
  jingleButton.setup(JINGLE_BUTTON);
  numpad.setup();
}

Input InputController::get() {
  if (startButton.pressed()) {
    return StdInput::START;
  }
  if (guessButton.pressed()) {
    return DiceInput::NEXT_GUESS;
  }
  if (jingleButton.pressed()) {
    return DiceInput::NEXT_JINGLE;
  }

  char ch = numpad.getChar();
  switch (ch) {
    case 'A':
      return StdInput::START;
    case 'B':
      return DiceInput::JINGLE;
    case 'C':
      return DiceInput::NEXT_JINGLE;
    case 'D':
      return StdInput::STOP;
    case '#':
      return StdInput::START;
    case '*':
      return DiceInput::NEXT_GUESS;
    default:
      break;
  }

  int num = ch - (int)'0';
  if (num >= 0 && num <= 9) {
    return (Input)num;
  }

  return StdInput::NONE;
}
