#pragma once

#include <ArduinoJson.h>
#include <ablib.h>

#include "const.h"
#include "storage.h"

class SelectionController {
 public:
  SelectionController(Storage& storage, ActivitySoundClient& sound, ActivityNumpadClient& numpad, ActivityDisplayClient& display);

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

  ActivitySoundClient& sound;
  ActivityNumpadClient& numpad;
  ActivityDisplayClient& display;

  int currentIndex = -1;
  long currentTimeout = 0;

  StoryId onStorySelected(int index);
};
