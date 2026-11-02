#pragma once

#include <Audio.h>
#include <rotaryencoder/rotaryencoder.h>
#include <storage/storage.h>

#include "core/core.h"
#include "ledring.h"

class AudioPlayer {
 public:
  AudioPlayer(Storage& storage, LedRing& ledRing);

  void setup();
  void loop();

  void play(const char* path);
  void stop();

  Audio audio;
  LedRing& ledRing;
  Storage& storage;
  ActivityRotaryEncoder volumeEncoder;

  bool mute;
  int volume;

  void setVolume(int volume);
  void toggleMute();

  static PlayerState playerState;
};
