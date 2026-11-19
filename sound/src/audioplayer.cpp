#include "audioplayer.h"

#include <base/base.h>

#include "const.h"

AudioPlayer::AudioPlayer(SDFS& sd)
    : sd(sd),
      mp3(nullptr),
      file(nullptr),
	  out(nullptr) {
}

void AudioPlayer::setup() {
  pinMode(AUDIO_BUSY, OUTPUT);
  signalBusy();

  out = new AudioOutputI2S();
  out->SetPinout(AUDIO_BCLK, AUDIO_LRC, AUDIO_DIN);
  out->SetGain(.3);

  LOG("audio player initialized");
}

void AudioPlayer::loop() {
  if (mp3) {
    if (mp3->isRunning()) {
      if (!mp3->loop()) {
        stop();
      }
    }
  }
}

bool AudioPlayer::play(const char* path) {
  stop();
  if (!sd.exists(path)) {
    signalBusy();
    LOG("cannot find track: path=%s", path);
    return false;
  }

  LOG("play - file");
  file = new AudioFileSourceSD(path);

  LOG("play - mp3");
  mp3 = new AudioGeneratorMP3();

  LOG( "play - begin");
  mp3->begin(file, out);
  
  LOG("playing track: path=%s", path);
  signalBusy();
  
  return true;
}

void AudioPlayer::stop() {
	LOG("stopping");
  if (mp3 && mp3->isRunning()) {
	LOG("mp3 running - stop it");
    mp3->stop();
  }
  cleanup();
  signalBusy();
  LOG("stopped");
}

void AudioPlayer::setVolume(int volume) {
  volume = min(max(volume, 0), 100);
  float gain = volume / 100.0;
  out->SetGain(gain);
}

void AudioPlayer::cleanup() {
  delete mp3;
  delete file;

  mp3 = nullptr;
  file = nullptr;
}

void AudioPlayer::signalBusy() {
  bool busy = mp3 && mp3->isRunning();
  digitalWrite(AUDIO_BUSY, busy ? LOW : HIGH);
}
