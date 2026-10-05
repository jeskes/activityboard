#include "storycontroller.h"

StoryController::StoryController(Storage& storage, ActivitySoundClient& sound, ActivityNumpadClient& numpad, ActivityDisplayClient& display)
    : storage(storage),
      sound(sound),
      numpad(numpad),
      display(display) {
}

void StoryController::loop() {
  if (chapterTrackStopped) {
    chapterTrackStopped = false;
    onChapterEnded();
  }

  if (isBusy()) {
    char ch = numpad.getChar();
    if (ch == '#') {
      LOG("STOP selected at numpad.");
      stop();
    }
    else if (ch == '*') {
      LOG("NEXT selected at numpad.");
	  sound.stop();
    }
    else if (menuSelectionPending) {
      uint8_t idx = numpad.getChar() - (int)'1';
      if (idx >= 0 && idx < menuSelectionPending) {
        LOG("menu option %d selected at numpad.", idx + 1);
        onMenuSelected(idx);
      }
    }
  }
}

bool StoryController::run(const char* story) {
  activeStory = story;
  if (!loadDefinition()) {
    activeStory = nullptr;
    return false;
  }

  const char* startChapter = definition["start"].as<const char*>();
  if (!startChapter) {
    activeStory = nullptr;
    return false;
  }

  runChapter(startChapter);

  return true;
}

void StoryController::runChapter(const char* chapterId) {
  currentChapter = chapterId;
  JsonObject chapterDef = definition["chapters"][currentChapter].as<JsonObject>();

  BitmapId bitmap = chapterDef["bitmap"].as<const char*>();
  display.displayBitmap(bitmap);

  TrackId track = chapterDef["track"].as<int>();
  chapterTrackStopped = false;
  sound.play(track, this);
}

void StoryController::onChapterEnded() {
  if (!currentChapter) {
    return;
  }
  JsonObject chapterDef = definition["chapters"][currentChapter].as<JsonObject>();

  if (chapterDef["menu"].is<JsonObject>()) {
    JsonObject menuDef = chapterDef["menu"].as<JsonObject>();
    MenuId menuId = menuDef["id"].as<const char*>();
    display.displayMenu(menuId);
    menuSelectionPending = menuDef["options"].as<JsonArray>().size();
  }
  else if (chapterDef["next"].is<const char*>()) {
    const char* nextChapter = chapterDef["next"].as<const char*>();
    runChapter(nextChapter);
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
  const char* nextChapter = options[selectedIndex].as<const char*>();
  runChapter(nextChapter);
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
  sound.stop();
  display.displayBitmap(BITMAP_STARTUP);
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
