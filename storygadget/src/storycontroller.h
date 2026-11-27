#pragma once

#include <modules/coreclient.h>

#include "const.h"
#include "definition.h"
#include "inputcontroller.h"

class StoryController : CoreStatusHandler {
 public:
  StoryController(Definition& definition, ActivityCoreClient& core, InputController& input);

  void setup();
  void loop();

  bool run(StoryDef* storyDef);
  bool stop();
  bool isBusy();
  bool inMenu();

 private:
  Definition& definition;
  ActivityCoreClient& core;
  InputController& inputController;

  StoryDef* activeStory;
  ChapterDef* currentChapter;
  void startChapter(ChapterId chapterId);
  void onChapterEnded();

  MenuDef* currentMenu;
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

  void playStoryTrack(TrackPath path);
};