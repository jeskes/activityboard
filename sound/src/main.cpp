#include "sound.h"

SoundService soundService;

void setup() {
  soundService.setup(SOUND_MODULE_ID);
  LOG("Sound Module started.");
}

void loop() {
  soundService.loop();
}
