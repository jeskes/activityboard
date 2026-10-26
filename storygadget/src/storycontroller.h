#pragma once

#include <core/core.h>
#include <numpad/numpad.h>
#include <storage/storage.h>

#include "const.h"

class StoryController : CoreStatusHandler {
 public:
  StoryController(Storage& storage, ActivityCoreClient& core, ActivityNumpadClient& numpad);

  bool run(StoryId story);
  void loop();
  bool isBusy();

 private:
  Storage& storage;
  JsonDocument definition;
  bool loadDefinition();

  ActivityCoreClient& core;
  ActivityNumpadClient& numpad;

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
