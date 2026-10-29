#pragma once

#include <ArduinoJson.h>
#include <core/core.h>
#include <storage/storage.h>

#include "const.h"
#include "inputcontroller.h"

class SelectionController {
 public:
  SelectionController(Storage& storage, ActivityCoreClient& core, InputController& input);

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
  InputController& input;

  int currentIndex = -1;
  long currentTimeout = 0;

  void initCurrentSelection();
  StoryId onStorySelected(int index);
};
