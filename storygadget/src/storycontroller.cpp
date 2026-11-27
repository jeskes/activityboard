#include "storycontroller.h"

StoryController::StoryController(Definition& definition, ActivityCoreClient& core, InputController& inputController)
    : definition(definition),
      core(core),
      inputController(inputController),
      playerMonitor(PlayerMonitor::UNDEFINED) {
}

void StoryController::setup() {
}

bool StoryController::run(StoryDef* storyDef) {
  activeStory = storyDef;
  currentChapter = nullptr;
  playerMonitor = PlayerMonitor::UNDEFINED;

  ChapterId start = activeStory->getStart();
  if (!start) {
    LOG("error: no start found");
    activeStory = nullptr;
    return false;
  }

  startChapter(start);
  core.setStatusHandler(this);

  return true;
}

void StoryController::loop() {
  if (playerMonitor == PlayerMonitor::ENDED) {
    playerMonitor = PlayerMonitor::UNDEFINED;
    if (inMenu()) {
      startMenuOption(currentMenuSelection);
      menuSelectionTimeout = millis() + SELECTION_TIMEOUT;
    }
    else {
      onChapterEnded();
    }
  }

  Input input = inputController.get();

  if (input == StdInput::STOP) {
    LOG("stop pressed");
    stop();
  }
  else if (currentMenu) {
    int currentMenuSize = currentMenu->getMenuSize();
    if (input >= StdInput::NUM_1 && (int)input <= currentMenuSize) {
      LOG("menu option %d selected at numpad.", input);
      onMenuOptionSelected(((int)input - 1));
    }
    else if (input == StdInput::START) {
      onMenuOptionSelected(currentMenuSelection);
    }
    else if (input == StdInput::REPEAT) {
      startMenu(currentMenu->getId());
    }
    else if (input == StdInput::NEXT) {
      menuSelectionTimeout = 0;
      startMenuOption((currentMenuSelection + 1) % currentMenuSize);
    }
    else if (menuSelectionTimeout && millis() > menuSelectionTimeout) {
      menuSelectionTimeout = millis() + SELECTION_TIMEOUT;
      startMenuOption((currentMenuSelection + 1) % currentMenuSize);
    }
  }
  else if (input == StdInput::REPEAT) {
    LOG("repeat pressed");
    startChapter(currentChapter->getId());
  }
  else if (input == StdInput::NEXT) {
    LOG("next pressed");
    core.stopSound();
  }
}

void StoryController::startChapter(ChapterId chapterId) {
  if (!chapterId) {
    return;
  }
  LOG("start chapter : %s", chapterId);
  ChapterDef* chapter = activeStory->getChapter(chapterId);
  if (!chapter) {
    LOG("chapter definition not found");
    return;
  }

  currentChapter = chapter;

  ImagePath image = currentChapter->getImage();
  if (image) {
    core.displayImage(image);
  }
  else {
    LOG("chapter image not found: chapter=%s", chapterId);
  }

  TrackPath track = currentChapter->getTrack();
  if (track) {
    playStoryTrack(track);
    playerMonitor = PlayerMonitor::PENDING;
  }
  else {
    LOG("chapter track not found: chapter=%s", chapterId);
  }

  currentMenu = nullptr;
  currentMenuSelection = 0;
}

void StoryController::onChapterEnded() {
  if (!currentChapter) {
    return;
  }

  LOG("chapter ended: %s", currentChapter->getId());

  MenuId menuId = currentChapter->getMenu();
  if (menuId) {
    startMenu(menuId);
  }
  else {
    ChapterId nextChapterId = currentChapter->getNext();
    if (nextChapterId) {
      startChapter(nextChapterId);
    }
    else {
      stop();
    }
  }
}

void StoryController::startMenu(MenuId menuId) {
  MenuDef* menu = activeStory->getMenu(menuId);
  if (!menu) {
    LOG("cannot find menu: id=%s", menuId);
  }
  currentMenu = menu;
  currentMenuSelection = 0;

  core.displayMenu(currentMenu->getId(), -1);
  playStoryTrack(currentMenu->getTrack());
  playerMonitor = PlayerMonitor::PENDING;
}

void StoryController::startMenuOption(uint8_t index) {
  currentMenuSelection = index;
  core.displayMenu(currentMenu->getId(), index);
  TrackPath track = currentMenu->getOptionTrack(index);
  playStoryTrack(track);
}

void StoryController::onMenuOptionSelected(uint8_t index) {
  LOG("menu option selected: %d", index);
  ChapterId nextChapter = currentMenu->getOptionNext(index);
  startChapter(nextChapter);
  menuSelectionTimeout = 0;
}

bool StoryController::stop() {
  if (!activeStory) {
    return false;
  }

  activeStory = nullptr;
  core.stopSound();
  core.displayImage(STARTUP_IMAGE);
  core.setStatusHandler(nullptr);

  return true;
}

bool StoryController::isBusy() {
  return activeStory != 0;
}

bool StoryController::inMenu() {
  return currentMenu != nullptr;
}

void StoryController::playStoryTrack(TrackPath path) {
  core.playTrack(path);
}

void StoryController::handleCoreStatus(CoreStatus status) {
  if (playerMonitor == PlayerMonitor::PENDING && status.playerState == PlayerState::IDLE) {
    LOG("idle");
    playerMonitor = PlayerMonitor::ENDED;
  }
}