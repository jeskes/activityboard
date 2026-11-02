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

  // task on core 1 with priority 3 to run "over" loop()
  xTaskCreatePinnedToCore(audioTask, "AudioTask", 8192, NULL, 3, NULL, 1);

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
  storage.sdAccess([&]() { audio.connecttoFS(SD, path); });
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
    // audio needs control (sd card secured)
    STORAGE->sdAccess([&]() { AUDIO->loop(); });

    // yield control to system
    vTaskDelay(pdMS_TO_TICKS(2));
  }
}
