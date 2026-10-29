#include "selectioncontroller.h"

SelectionController::SelectionController(Storage& storage, ActivityCoreClient& core, InputController& input)
    : storage(storage),
      core(core),
      input(input) {
}

void SelectionController::setup() {
  loadStories();
}

uint8_t SelectionController::getStoriesCount() {
  return storiesCount;
}

StoryId SelectionController::getStory(uint8_t index) {
  return stories[index];
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
    if (currentIndex >= storiesCount) {
      currentIndex = -1;
      return nullptr;
    }
    initCurrentSelection();
  }

  int idx = input.numberPressed() - 1;
  if (idx >= 0 && idx < getStoriesCount()) {
    LOG("story %d selected", idx + 1);
    return onStorySelected(idx);
  }

  if (input.startPressed()) {
    LOG("story %d selected", currentIndex + 1);
    return onStorySelected(currentIndex);
  }

  if (input.repeatPressed()) {
    LOG("repeat selection");
    initCurrentSelection();
  }
  else if (input.skipPressed()) {
    LOG("skip selection");
    currentIndex = (currentIndex + 1) % storiesCount;
    initCurrentSelection();
  }
  else if (input.cancelPressed()) {
    LOG("selection cancelled");
    core.stopSound();
    currentIndex = -1;
  }

  return nullptr;
}

void SelectionController::loadStories() {
  if (!storage.loadJson("/stories.json", storiesDocument)) {
    LOG("cannot load stories");
    return;
  }

  storiesCount = 0;
  JsonObject entries = storiesDocument.as<JsonObject>();
  for (JsonPair entry : entries) {
    if (storiesCount < MAX_STORIES) {
      stories[storiesCount] = entry.key().c_str();
      storiesCount++;
    }
    else {
      break;
    }
  }
}

void SelectionController::initCurrentSelection() {
  currentTimeout = millis() + SELECTION_TIMEOUT;
  JsonObject storyDef = storiesDocument[stories[currentIndex]].as<JsonObject>();
  core.playSound(storyDef["track"].as<TrackId>());
  core.displayBitmap(storyDef["bitmap"].as<BitmapId>());
}

StoryId SelectionController::onStorySelected(int index) {
  core.stopSound();
  currentIndex = -1;
  return stories[index];
}