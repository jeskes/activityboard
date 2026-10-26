#include "selectioncontroller.h"

SelectionController::SelectionController(Storage& storage, ActivityCoreClient& core, ActivityNumpadClient& numpad)
    : storage(storage),
      core(core),
      numpad(numpad) {
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

    currentTimeout = millis() + SELECTION_TIMEOUT;
    JsonObject storyDef = storiesDocument[stories[currentIndex]].as<JsonObject>();
    core.playSound(storyDef["track"].as<TrackId>());
    core.displayBitmap(storyDef["bitmap"].as<BitmapId>());
  }

  int idx = numpad.getChar() - (int)'1';
  if (idx >= 0 && idx < MAX_STORIES && stories[idx]) {
    LOG("story %d selected at numpad.", idx + 1);
    return onStorySelected(idx);
  }

  if (activityControls.buttonPressed(SELECT_BUTTON)) {
    LOG("story %d selected using select button.", currentIndex + 1);
    return onStorySelected(currentIndex);
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

StoryId SelectionController::onStorySelected(int index) {
  core.stopSound();
  currentIndex = -1;
  return stories[index];
}