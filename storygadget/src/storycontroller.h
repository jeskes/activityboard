#pragma once

#include <core/core.h>
#include <numpad/numpad.h>
#include <storage/storage.h>

#include "const.h"
#include "inputcontroller.h"

class StoryController : CoreStatusHandler {
 public:
  StoryController(Storage& storage, ActivityCoreClient& core, InputController& input);

  void setup();
  bool run(StoryId story);
  void loop();
  bool isBusy();

 private:
  Storage& storage;
  JsonDocument definition;
  bool loadDefinition();

  ActivityCoreClient& core;
  InputController& input;

  StoryId activeStory;
  ChapterId currentChapter;

  MenuId currentMenuId;
  uint8_t currentMenuSize;
  ;
  uint8_t currentMenuSelection;

  bool next();
  bool stop();

  void startChapter(ChapterId chapter);
  void onChapterEnded();
  void onMenuSelected(uint8_t selectedIndex);

  enum class PlayerMonitor {
	UNDEFINED, PENDING, ENDED
  };
  PlayerMonitor playerMonitor;
  bool handleCoreStatus(CoreStatus status) override;
};
