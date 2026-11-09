#pragma once

#include <DFRobotDFPlayerMini.h>
#include <base/base.h>
#include <modules/requests.h>

#include <functional>

#include "ledring.h"
#include "rotaryexpansion.h"

using PlayerStateChangeHandler = std::function<void()>;

class Player {
 public:
  Player(LedRing& ledRing, RotaryExpansion& rotary);

  void setup();
  void loop();

  void play(TrackFolder folder, TrackId track);
  void stop();

  PlayerState playerState;
  void onStateChange(PlayerStateChangeHandler handler);

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

  PlayerStateChangeHandler handler;
};
