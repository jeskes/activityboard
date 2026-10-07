#pragma once

#include <ArduinoJson.h>

#include "const.h"
#include "storage.h"

class StoryController : SoundStatusHandler {
 public:
  StoryController(Storage& storage, ActivitySoundClient& sound, ActivityNumpadClient& numpad, ActivityDisplayClient& display);

  bool run(StoryId story);
  void loop();
  bool isBusy();

 private:
  Storage& storage;
  JsonDocument definition;
  bool loadDefinition();

  ActivitySoundClient& sound;
  ActivityNumpadClient& numpad;
  ActivityDisplayClient& display;

  StoryId activeStory;
  bool chapterTrackStopped;
  uint8_t menuSelectionPending;
  ChapterId currentChapter;
  
  bool next();
  bool stop();

  void startChapter(ChapterId chapter);
  void onChapterEnded();
  void onMenuSelected(uint8_t selectedIndex);
  bool playerStateChanged(PlayerState state);
};
