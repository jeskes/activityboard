#include "audioplayer.h"

#include <base/base.h>

#include "const.h"

AudioPlayer::AudioPlayer(SDFS& sd)
    : sd(sd),
      mp3(nullptr),
      file(nullptr),
      out(nullptr),
      currentGain(INITIAL_GAIN) {
}

void AudioPlayer::setup() {
  pinMode(AUDIO_BUSY, OUTPUT);
  signalBusy();

  out = new AudioOutputI2S();
  out->SetPinout(AUDIO_BCLK, AUDIO_LRC, AUDIO_DIN);
  out->SetGain(currentGain);

  LOG("audio player initialized");
}

void AudioPlayer::loop() {
  if (mp3 && mp3->isRunning()) {
    if (!paused) {
      if (!mp3->loop()) {
        stop();
      }
    }
    else {
      int16_t silence[2] = {0, 0};
      while (out->ConsumeSample(silence)) {
      }
      delay(5);
    }
  }
}

bool AudioPlayer::play(const char* path) {
  stop();
  paused = false;
  if (!path || !sd.exists(path)) {
    signalBusy();
    LOG("cannot find track: path=%s", path ? path : "<null>");
    return false;
  }

  out->SetGain(currentGain);
  file = new AudioFileSourceSD(path);
  mp3 = new AudioGeneratorMP3();
  mp3->begin(file, out);
  signalBusy();

  return true;
}

void AudioPlayer::pause() {
  LOG("pause");
  if (mp3 && mp3->isRunning()) {
    out->SetGain(0);
    delay(10);
    paused = true;
    signalBusy();
  }
}

void AudioPlayer::resume() {
  LOG("resume");
  if (mp3 && mp3->isRunning() && paused) {
    out->SetGain(currentGain);
    paused = false;
    signalBusy();
  }
}

void AudioPlayer::stop() {
  LOG("stopping");
  if (mp3 && mp3->isRunning()) {
    LOG("mp3 running - stop it");
    mp3->stop();
  }
  cleanup();
  signalBusy();
  paused = false;
  LOG("stopped");
}

void AudioPlayer::setVolume(int volume) {
  volume = min(max(volume, 0), 100);
  currentGain = volume / 100.0;
  out->SetGain(currentGain);
}

void AudioPlayer::cleanup() {
  delete mp3;
  delete file;

  mp3 = nullptr;
  file = nullptr;
}

void AudioPlayer::signalBusy() {
  bool busy = mp3 && mp3->isRunning() && !paused;
  digitalWrite(AUDIO_BUSY, busy ? LOW : HIGH);
}
