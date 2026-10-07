#include "selectioncontroller.h"

SelectionController::SelectionController(Storage& storage, ActivitySoundClient& sound, ActivityNumpadClient& numpad, ActivityDisplayClient& display)
    : storage(storage),
      sound(sound),
      numpad(numpad),
      display(display) {
}

void SelectionController::setup() {
  loadStories();
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
    sound.play(storyDef["track"].as<TrackId>());
    display.displayBitmap(storyDef["bitmap"].as<BitmapId>());
  }

  int idx = numpad.getChar() - (int)'1';
  if (idx >= 0 && idx < MAX_STORIES && stories[idx]) {
    LOG("story %d selected at numpad.", idx + 1);
    return onStorySelected(idx);
  }

  if (activityControls.buttonPressed(SELECT_BUTTON)) {
    LOG("story %d selected at buttons.", currentIndex + 1);
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
  sound.stop();
  currentIndex = -1;
  return stories[index];
}