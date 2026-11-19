#include "soundservice.h"

static SoundService soundService;

unsigned long startup = millis() + 500;

void setup() {
  soundService.setup();
}

void loop() {
  soundService.loop();
  if (startup && millis() > startup) {
    soundService.audioPlayer.play("/10/100.mp3");
    startup = 0;
  }
}
