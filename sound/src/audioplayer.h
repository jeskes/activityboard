#pragma once

#include <SD.h>

#include "AudioFileSourceSD.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"

class AudioPlayer {
 public:
  AudioPlayer(SDFS& sd);

  void setup();
  void loop();

  bool play(const char* path);
  void pause();
  void resume();
  void stop();
  void setVolume(int volume);

 private:
  SDFS& sd;
  AudioGeneratorMP3* mp3;
  AudioFileSourceSD* file;
  AudioOutputI2S* out;

  bool paused;
  float currentGain;

  void cleanup();
  void signalBusy();
};
