#pragma once

#include <ESP32Encoder.h>

#include "DFRobotDFPlayerMini.h"
#include "core/core.h"
#include "modules/modules.h"
#include "rotaryencoder/rotaryencoder.h"
#include "ledring.h"

class Player {
 public:
  Player(LedRing& ledRing);

  void setup();
  void loop();

  void play(TrackFolder folder, TrackId track);
  void stop();

  static PlayerState playerState;

 private:
  bool mute;
  int volume;

  LedRing& ledRing;
  HardwareSerial playerSerial;
  DFRobotDFPlayerMini player;
  ActivityRotaryEncoder volumeEncoder;

  bool checkPlaying();
  void awaitBusy(bool playing);
  void setVolume(int volume);
  void toggleMute();
};
