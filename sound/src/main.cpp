#include "sound.h"

SoundService soundService;

void setup() {
  soundService.setup(SOUND_MODULE_ID);
  soundService.play(SYSTEM_CLIENT_ID, STARTUP_TRACK_ID);
  LOG("Sound Module started.");
}

void loop() {
  soundService.loop();
}
