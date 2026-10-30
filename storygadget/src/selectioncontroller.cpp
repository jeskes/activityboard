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

  Input in = input.get();

  if (in == Input::START) {
    LOG("story %d selected", currentIndex + 1);
    return onStorySelected(currentIndex);
  }
  else if (in >= Input::NUM_1 && (int)in <= storiesCount) {
    LOG("story %d selected", in);
    return onStorySelected((int)in - 1);
  }
  else if (in == Input::REPEAT) {
    LOG("repeat selection");
    initCurrentSelection();
  }
  else if (in == Input::NEXT) {
    LOG("next selection");
    currentIndex = (currentIndex + 1) % storiesCount;
    initCurrentSelection();
  }
  else if (in == Input::CANCEL) {
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