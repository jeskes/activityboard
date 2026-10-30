#include "storycontroller.h"

StoryController::StoryController(Storage& storage, ActivityCoreClient& core, InputController& input)
    : storage(storage),
      core(core),
      input(input),
      playerMonitor(PlayerMonitor::UNDEFINED) {
}

void StoryController::setup() {
}

bool StoryController::run(StoryId story) {
  activeStory = story;
  currentChapter = nullptr;
  playerMonitor = PlayerMonitor::UNDEFINED;

  if (!loadDefinition()) {
    activeStory = nullptr;
    return false;
  }

  ChapterId start = definition["start"].as<ChapterId>();
  if (!start) {
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
    }
    else {
      onChapterEnded();
    }
  }

  Input in = input.get();

  if (in == Input::CANCEL) {
    LOG("cancel pressed");
    stop();
  }
  else if (currentMenu) {
    if (in >= Input::NUM_1 && (int)in <= currentMenuSize) {
      LOG("menu option %d selected at numpad.", in);
      onMenuOptionSelected(((int)in - 1));
    }
    else if (in == Input::START) {
      onMenuOptionSelected(currentMenuSelection);
    }
    else if (in == Input::REPEAT) {
      startMenu(currentMenu);
    }
    else if (in == Input::NEXT) {
      startMenuOption((currentMenuSelection + 1) % currentMenuSize);
    }
  }
  else if (in == Input::REPEAT) {
    LOG("repeat pressed");
    startChapter(currentChapter);
  }
  else if (in == Input::NEXT) {
    LOG("next pressed");
    core.stopSound();
  }
}

void StoryController::startChapter(ChapterId chapter) {
  LOG("start chapter : %s", chapter);
  currentChapter = chapter;
  JsonObject chapterDef = definition["chapters"][currentChapter].as<JsonObject>();

  BitmapId bitmap = chapterDef["bitmap"].as<BitmapId>();
  core.displayBitmap(bitmap);

  TrackId track = chapterDef["track"].as<TrackId>();
  core.playSound(tracks, track);
  playerMonitor = PlayerMonitor::PENDING;

  currentMenu = nullptr;
  currentMenuSize = 0;
  currentMenuSelection = 0;
}

void StoryController::onChapterEnded() {
  if (!currentChapter) {
    return;
  }

  LOG("chapter ended: %s", currentChapter);
  JsonObject chapterDef = definition["chapters"][currentChapter].as<JsonObject>();

  if (chapterDef["menu"].is<MenuId>()) {
    startMenu(chapterDef["menu"].as<MenuId>());
  }
  else if (chapterDef["next"].is<ChapterId>()) {
    ChapterId nextChapter = chapterDef["next"].as<ChapterId>();
    startChapter(nextChapter);
  }
  else {
    stop();
  }
}

void StoryController::startMenu(MenuId menu) {
  currentMenu = menu;
  currentMenuDef = definition["menus"][currentMenu].as<JsonObject>();
  currentMenuSize = currentMenuDef["options"].as<JsonArray>().size();
  currentMenuSelection = 0;

  core.displayMenu(currentMenu, currentMenuSelection);
  core.playSound(tracks, currentMenuDef["track"].as<TrackId>());
  playerMonitor = PlayerMonitor::PENDING;
}

void StoryController::startMenuOption(uint8_t index) {
  currentMenuSelection = index;
  core.displayMenu(currentMenu, index);
  JsonObject option = currentMenuDef["options"].as<JsonArray>()[index].as<JsonObject>();
  TrackId track = option["track"].as<TrackId>();
  core.playSound(tracks, track);
}

void StoryController::onMenuOptionSelected(uint8_t index) {
  JsonObject option = currentMenuDef["options"].as<JsonArray>()[index].as<JsonObject>();
  ChapterId nextChapter = option["next"].as<ChapterId>();
  startChapter(nextChapter);
}

bool StoryController::stop() {
  if (!activeStory) {
    return false;
  }

  activeStory = nullptr;
  core.stopSound();
  core.displayBitmap(STARTUP_BITMAP);
  core.setStatusHandler(nullptr);

  return true;
}

bool StoryController::loadDefinition() {
  char path[20];
  sprintf(path, "/%s.json", activeStory);
  if (!storage.loadJson(path, definition)) {
    LOG("cannot load story");
    return false;
  }
  tracks = definition["tracks"].as<TrackFolder>();
  return true;
}

bool StoryController::isBusy() {
  return activeStory != 0;
}

bool StoryController::inMenu() {
  return currentMenu != nullptr;
}

bool StoryController::handleCoreStatus(CoreStatus status) {
  if (playerMonitor == PlayerMonitor::PENDING && status.playerState == PlayerState::IDLE) {
    LOG("idle");
    playerMonitor = PlayerMonitor::ENDED;
    return true;
  }
  return false;
}
