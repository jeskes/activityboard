#include "audioplayer.h"

#include <base/base.h>

#include "const.h"

static Audio* AUDIO = nullptr;
static Storage* STORAGE = nullptr;
static void audioTask(void* pvParameters);

PlayerState AudioPlayer::playerState = PlayerState::UNKNOWN;

AudioPlayer::AudioPlayer(Storage& storage, LedRing& ledRing)
    : storage(storage),
      ledRing(ledRing),
      mute(false),
      volumeEncoder(INITIAL_VOLUME, MINIMUM_VOLUME, MAXIMUM_VOLUME),
      volume(INITIAL_VOLUME) {
  AUDIO = &audio;
  STORAGE = &storage;
}

void AudioPlayer::setup() {
  audio.setBufsize(16384, 0);
  audio.setPinout(PLAYER_BCLK, PLAYER_LRC, PLAYER_DIN);
  audio.setVolume(volume);

  // task on core 1 with priority 5 to run "over" loop()
  xTaskCreatePinnedToCore(audioTask, "AudioTask", 8192, NULL, 5, NULL, 1);

  volumeEncoder.setup(VOLUME_CLK, VOLUME_DT, VOLUME_SW);
  volumeEncoder.setHandler([this](long volume) { this->setVolume(volume); });

  LOG("successfully initialized player");
}

void AudioPlayer::loop() {
  volumeEncoder.loop();
  if (volumeEncoder.pressed()) {
    toggleMute();
  }
}

void AudioPlayer::play(const char* path) {
  LOG("play track: %s", path);
  playerState = PlayerState::UNKNOWN;
  audio.setVolume(0);
  storage.sdAccess([&]() { audio.connecttoFS(SD, path); });
  delay(80);

  int startVol = volume / 4;
  if (startVol < 2)
    startVol = 2;
  for (int v = startVol; v <= volume; v += startVol) {
    audio.setVolume(v);
    delay(15);
  }
  audio.setVolume(volume);
}

void AudioPlayer::stop() {
  LOG("stop track");
  storage.sdAccess([&]() { audio.stopSong(); });
}

void AudioPlayer::setVolume(int volume) {
  LOG("set volume: vol=%d", volume);
  mute = false;
  audio.setVolume(volume);
  this->volume = volume;
  ledRing.updateVolume(volume, mute);
}

void AudioPlayer::toggleMute() {
  mute = !mute;
  LOG("toggle mute: mute=%d", (int)mute);
  audio.setVolume(mute ? 0 : volume);
  ledRing.updateVolume(volume, mute);
}

/* ========================================================================= */
/*                                                           audio callbacks */
/* audio_showstation : called when track was opened successfully             */
/* audio_eof_mp3     : called when track ended                               */
/* audio_error_mp3   : called when an error occurred                         */
/* ========================================================================= */

void audio_showstation(const char* info) {
  LOG("audio playing: %s", info);
  AudioPlayer::playerState = PlayerState::PLAYING;
}

void audio_eof_mp3(const char* info) {
  LOG("audio ended: %s", info);
  AudioPlayer::playerState = PlayerState::IDLE;
}

void audio_error_mp3(const char* info) {
  LOG("audio error: %s", info);
  AudioPlayer::playerState = PlayerState::IDLE;
}

static void audioTask(void* pvParameters) {
  while (1) {
    if (AudioPlayer::playerState == PlayerState::PLAYING || AudioPlayer::playerState == PlayerState::UNKNOWN) {
      STORAGE->sdAccess([&]() { AUDIO->loop(); });
    }
    else {
      AUDIO->loop();
    }
    // Minimales Nachgeben an den Scheduler, damit andere Tasks atmen können
    vTaskDelay(1);
  }
}