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

bool InputController::selectPressed() {
	return selectButton.pressed();
}

bool InputController::startPressed() {
	return startButton.pressed() ||  numpad.getChar() == 'D';
}

bool InputController::repeatPressed() {
	return repeatButton.pressed() ||  numpad.getChar() == 'A';
}

bool InputController::skipPressed() {
	return skipButton.pressed() ||  numpad.getChar() == 'B';
}

bool InputController::cancelPressed() {
	return cancelButton.pressed() ||  numpad.getChar() == 'C';
}

int InputController::numberPressed() {
  int idx = numpad.getChar() - (int)'0';
  return idx >= 0 && idx <= 9 ? idx : -1;
}
