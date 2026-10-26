#pragma once

#include <Adafruit_NeoPixel.h>

class LedRing {
 public:
  LedRing();

  void setup();

  void updateVolume(int volume, bool mute);
  void updateBacklight(int brightness);

 private:
  Adafruit_NeoPixel rings;
};
