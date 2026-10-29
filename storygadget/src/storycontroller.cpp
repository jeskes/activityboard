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
    onChapterEnded();
  }

  if (input.cancelPressed()) {
    LOG("cancel pressed");
    stop();
  }
  else if (input.skipPressed()) {
    LOG("skip pressed");
    core.stopSound();
  }
  else if (input.repeatPressed()) {
    LOG("repeat pressed");
    startChapter(currentChapter);
  }
  else if (currentMenuId) {
    uint8_t idx = input.numberPressed() - 1;
    if (idx >= 0 && idx < currentMenuSize) {
      LOG("menu option %d selected at numpad.", idx + 1);
      onMenuSelected(idx);
    }
    else if (input.selectPressed()) {
      currentMenuSelection = (currentMenuSelection + 1) % currentMenuSize;
      core.displayMenu(currentMenuId, currentMenuSelection);
    }
    else if (input.startPressed()) {
      onMenuSelected(currentMenuSelection);
    }
  }
}

void StoryController::startChapter(ChapterId chapter) {
  LOG("start chapter : %s", chapter);
  currentChapter = chapter;
  JsonObject chapterDef = definition["chapters"][currentChapter].as<JsonObject>();

  BitmapId bitmap = chapterDef["bitmap"].as<BitmapId>();
  core.displayBitmap(bitmap);

  TrackId track = chapterDef["track"].as<TrackId>();
  core.playSound(track);
  playerMonitor = PlayerMonitor::PENDING;

  currentMenuId = nullptr;
  currentMenuSize = 0;
  currentMenuSelection = 0;
}

void StoryController::onChapterEnded() {
  if (!currentChapter) {
    return;
  }

  LOG("chapter ended: %s", currentChapter);
  JsonObject chapterDef = definition["chapters"][currentChapter].as<JsonObject>();

  if (chapterDef["menu"].is<JsonObject>()) {
    JsonObject menuDef = chapterDef["menu"].as<JsonObject>();

    currentMenuId = menuDef["id"].as<MenuId>();
    currentMenuSize = menuDef["options"].as<JsonArray>().size();
    currentMenuSelection = 0;
    core.displayMenu(currentMenuId, currentMenuSelection);

    // maybe explicitly define track for menu
    // TrackId track = menuDef["id"].as<TrackId>();
    // core.playSound(track);
  }
  else if (chapterDef["next"].is<ChapterId>()) {
    ChapterId nextChapter = chapterDef["next"].as<ChapterId>();
    startChapter(nextChapter);
  }
  else {
    stop();
  }
}

void StoryController::onMenuSelected(uint8_t selectedIndex) {
  if (!currentChapter) {
    return;
  }
  JsonObject chapterDef = definition["chapters"][currentChapter].as<JsonObject>();
  if (!chapterDef["menu"].is<JsonObject>()) {
    return;
  }
  JsonObject menuDef = chapterDef["menu"].as<JsonObject>();
  JsonArray options = menuDef["options"].as<JsonArray>();
  ChapterId nextChapter = options[selectedIndex].as<ChapterId>();
  startChapter(nextChapter);
}

bool StoryController::next() {
  LOG("next ")
  if (!currentChapter) {
    return false;
  }
  onChapterEnded();
  return true;
}

bool StoryController::stop() {
  if (!activeStory) {
    return false;
  }

  activeStory = nullptr;
  core.stopSound();
  core.displayBitmap(BITMAP_STARTUP);
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
  return true;
}

bool StoryController::isBusy() {
  return activeStory != 0;
}

bool StoryController::handleCoreStatus(CoreStatus status) {
  if (playerMonitor == PlayerMonitor::PENDING && status.playerState == PlayerState::IDLE) {
    LOG("idle");
    playerMonitor = PlayerMonitor::ENDED;
    return true;
  }
  return false;
}
