#include "selectioncontroller.h"

SelectionController::SelectionController(Definition& definition, ActivityCoreClient& core, InputController& inputController)
    : definition(definition),
      core(core),
      inputController(inputController) {
}

void SelectionController::setup() {
}

void SelectionController::run() {
  LOG("run selection");
  currentIndex = -1;
  currentTimeout = 0;
  loop();
}

void SelectionController::stop() {
  core.stopSound();
  currentIndex = -1;
}

bool SelectionController::isBusy() {
  return currentIndex >= 0;
}

StoryId SelectionController::loop() {
  if (millis() > currentTimeout) {
    currentIndex++;
    if (currentIndex >= definition.storiesCount) {
      currentIndex = -1;
      return nullptr;
    }
    initCurrentSelection();
  }

  Input input = inputController.get();

  if (input == StdInput::START) {
    LOG("story %d selected", currentIndex + 1);
    return onStorySelected(currentIndex);
  }
  else if (input >= StdInput::NUM_1 && (int)input <= definition.storiesCount) {
    LOG("story %d selected", input);
    return onStorySelected((int)input - 1);
  }
  else if (input == StdInput::REPEAT) {
    LOG("repeat selection");
    initCurrentSelection();
  }
  else if (input == StdInput::NEXT) {
    LOG("next selection");
    currentIndex = (currentIndex + 1) % definition.storiesCount;
    initCurrentSelection();
  }
  else if (input == StdInput::STOP) {
    LOG("selection stopped");
    stop();
  }

  return nullptr;
}

void SelectionController::initCurrentSelection() {
  currentTimeout = millis() + SELECTION_TIMEOUT;
  JsonObject storyInfo = definition.getStoryInfo(currentIndex);
  core.playSound(storyInfo["track"].as<TrackId>());
  core.displayImage(storyInfo["image"].as<ImageId>());
}

StoryId SelectionController::onStorySelected(int index) {
  stop();
  return definition.getStoryId(index);
}