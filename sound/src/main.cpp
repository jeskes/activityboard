#include <Arduino.h>
#include <Wire.h>
#include <ablib.h>

#include "sound.h"

SoundService soundService;

void setup() {
  soundService.setup();
  LOG("Sound Module started.");
}

void loop() {
	soundService.loop();
  // delay(300);
}
