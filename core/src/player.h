#pragma once

#include "DFRobotDFPlayerMini.h"
#include "base/base.h"
#include "modules/requests.h"
#include "ledring.h"
#include "rotaryexpansion.h"

class Player {
 public:
  Player(LedRing& ledRing, RotaryExpansion& rotary);

  void setup();
  void loop();

  void play(TrackFolder folder, TrackId track);
  void stop();

  static PlayerState playerState;

 private:
  bool mute;
  int volume;

  LedRing& ledRing;
  RotaryAdapter volumeAdapter;
  HardwareSerial playerSerial;
  DFRobotDFPlayerMini player;

  bool checkPlaying();
  void awaitBusy(bool playing);
  void setVolume(int volume);
  void toggleMute();
};
