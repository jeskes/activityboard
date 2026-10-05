#ifndef _AB_STORYCONTROLLER
#define AB_STORYCONTROLLER

#include <ArduinoJson.h>

#include "const.h"
#include "storage.h"

class StoryController : public SoundStatusHandler {
 public:
  StoryController(Storage& storage, ActivitySoundClient& sound, ActivityNumpadClient& numpad, ActivityDisplayClient& display);

  void loop();

  bool run(const char* story);
  bool next();
  bool stop();
  bool isBusy();

  const char* activeStory;
  bool chapterTrackStopped;
  uint8_t menuSelectionPending;
  const char* currentChapter;
  bool playerStateChanged(PlayerState state);

  ActivitySoundClient& sound;
  ActivityNumpadClient& numpad;
  ActivityDisplayClient& display;

 private:
  Storage& storage;
  JsonDocument definition;
  bool loadDefinition();

  void runChapter(const char* chapter);
  void onChapterEnded();
  void onMenuSelected(uint8_t selectedIndex);
};

#endif
