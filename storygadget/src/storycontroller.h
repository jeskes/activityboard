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
  bool inMenu();

 private:
  Storage& storage;
  ActivityCoreClient& core;
  InputController& input;

  JsonDocument definition;
  bool loadDefinition();
  TrackFolder tracks;

  StoryId activeStory;
  bool stop();

  ChapterId currentChapter;
  void startChapter(ChapterId chapter);
  void onChapterEnded();

  MenuId currentMenu;
  JsonObject currentMenuDef;
  uint8_t currentMenuSize;
  uint8_t currentMenuSelection;
  void startMenu(MenuId menu);
  void startMenuOption(uint8_t index);
  void onMenuOptionSelected(uint8_t index);

  enum class PlayerMonitor {
    UNDEFINED,
    PENDING,
    ENDED
  };
  PlayerMonitor playerMonitor;
  bool handleCoreStatus(CoreStatus status) override;
};
