#pragma once

#include <modules/coreclient.h>
#include <numpad/numpad.h>

#include "const.h"
#include "definition.h"
#include "inputcontroller.h"

class StoryController : CoreStatusHandler {
 public:
  StoryController(Definition& definition, ActivityCoreClient& core, InputController& input);

  void setup();
  void loop();

  bool run(StoryId story);
  bool stop();
  bool isBusy();
  bool inMenu();

 private:
  Definition& definition;
  ActivityCoreClient& core;
  InputController& inputController;

  StoryId activeStory;
  JsonVariant storyDefinition;
  TrackFolder tracks;

  ChapterId currentChapter;
  void startChapter(ChapterId chapter);
  void onChapterEnded();

  MenuId currentMenu;
  JsonObject currentMenuDef;
  uint8_t currentMenuSize;
  uint8_t currentMenuSelection;
  long menuSelectionTimeout;

  void startMenu(MenuId menu);
  void startMenuOption(uint8_t index);
  void onMenuOptionSelected(uint8_t index);

  enum class PlayerMonitor {
    UNDEFINED,
    PENDING,
    ENDED
  };
  PlayerMonitor playerMonitor;
  void handleCoreStatus(CoreStatus status) override;
};