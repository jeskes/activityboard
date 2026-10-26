#include "storycontroller.h"

StoryController::StoryController(Storage& storage, ActivityCoreClient& core, ActivityNumpadClient& numpad)
    : storage(storage),
      core(core),
      numpad(numpad) {
}

void StoryController::loop() {
  if (chapterTrackStopped) {
    chapterTrackStopped = false;
    onChapterEnded();
  }

  char ch = numpad.getChar();
  if (ch == '#') {
    LOG("STOP selected at numpad.");
    stop();
  }
  else if (ch == '*') {
    LOG("NEXT selected at numpad.");
    core.stopSound();
  }
  else if (ch == '0') {
    LOG("REPLAY (0) selected at numpad.");
    startChapter(currentChapter);
  }
  else if (menuSelectionPending) {
    uint8_t idx = ch - (int)'1';
    if (idx >= 0 && idx < menuSelectionPending) {
      LOG("menu option %d selected at numpad.", idx + 1);
      onMenuSelected(idx);
    }
  }
}

bool StoryController::run(StoryId story) {
  activeStory = story;
  currentChapter = nullptr;
  chapterTrackStopped = false;
  menuSelectionPending = false;
  core.setStatusHandler(this);

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

  return true;
}

void StoryController::startChapter(ChapterId chapter) {
  LOG("start chapter : %s", chapter);
  currentChapter = chapter;
  JsonObject chapterDef = definition["chapters"][currentChapter].as<JsonObject>();

  BitmapId bitmap = chapterDef["bitmap"].as<BitmapId>();
  core.displayBitmap(bitmap);

  TrackId track = chapterDef["track"].as<TrackId>();
  chapterTrackStopped = false;
  core.playSound(track);
}

void StoryController::onChapterEnded() {
  if (!currentChapter) {
    return;
  }
  JsonObject chapterDef = definition["chapters"][currentChapter].as<JsonObject>();

  if (chapterDef["menu"].is<JsonObject>()) {
    JsonObject menuDef = chapterDef["menu"].as<JsonObject>();

    MenuId menu = menuDef["id"].as<MenuId>();
    core.displayMenu(menu);
    menuSelectionPending = menuDef["options"].as<JsonArray>().size();

    TrackId track = menuDef["id"].as<TrackId>();
    core.playSound(track);
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

bool StoryController::playerStateChanged(PlayerState playerState) {
  if (playerState == PlayerState::IDLE) {
    chapterTrackStopped = true;
    return true;
  }
  return false;
}
