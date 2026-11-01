#include "selectioncontroller.h"

SelectionController::SelectionController(Definition& definition, ActivityCoreClient& core, InputController& input)
    : definition(definition),
      core(core),
      input(input) {
}

void SelectionController::setup() {
}

void SelectionController::run() {
  currentIndex = -1;
  currentTimeout = 0;
  loop();
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

  Input in = input.get();

  if (in == Input::START) {
    LOG("story %d selected", currentIndex + 1);
    return onStorySelected(currentIndex);
  }
  else if (in >= Input::NUM_1 && (int)in <= definition.storiesCount) {
    LOG("story %d selected", in);
    return onStorySelected((int)in - 1);
  }
  else if (in == Input::REPEAT) {
    LOG("repeat selection");
    initCurrentSelection();
  }
  else if (in == Input::NEXT) {
    LOG("next selection");
    currentIndex = (currentIndex + 1) % definition.storiesCount;
    initCurrentSelection();
  }
  else if (in == Input::CANCEL) {
    LOG("selection cancelled");
    core.stopSound();
    currentIndex = -1;
  }

  return nullptr;
}

void SelectionController::initCurrentSelection() {
  currentTimeout = millis() + SELECTION_TIMEOUT;
  JsonObject storyInfo = definition.getStoryInfo(currentIndex);
  core.playSound(storyInfo["track"].as<TrackId>());
  core.displayBitmap(storyInfo["bitmap"].as<BitmapId>());
}

StoryId SelectionController::onStorySelected(int index) {
  core.stopSound();
  currentIndex = -1;
  return definition.getStoryId(index);
}