#pragma once

#include <ArduinoJson.h>
#include <core/core.h>
#include <numpad/numpad.h>
#include <storage/storage.h>

#include "const.h"

class SelectionController {
 public:
  SelectionController(Storage& storage, ActivityCoreClient& core, ActivityNumpadClient& numpad);

  void setup();

  void run();
  StoryId loop();
  bool isBusy();

  uint8_t getStoriesCount();
  StoryId getStory(uint8_t index);

 private:
  uint8_t storiesCount;
  StoryId stories[MAX_STORIES];

 private:
  Storage& storage;
  void loadStories();
  JsonDocument storiesDocument;

  ActivityCoreClient& core;
  ActivityNumpadClient& numpad;

  int currentIndex = -1;
  long currentTimeout = 0;

  StoryId onStorySelected(int index);
};
