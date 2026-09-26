#include "sound.h"

#include <Arduino.h>
#include <ablib.h>

// pins

#define VOLUME_SW 2
#define VOLUME_DT 3
#define VOLUME_CLK 4
#define PLAYER_LED 9
#define PLAYER_TX 10
#define PLAYER_RX 11
#define PLAYER_BUSY 12

#define INITIAL_VOLUME 5
#define MAXIMUM_VOLUME 25

#define WAIT_FOR_BUSY_COUNT 5
#define WAIT_FOR_BUSY_TIMEOUT 300

static long volumeEncoderPosition = -999;

PeriphericalService* PeriphericalService::instance = nullptr;

// pins at arduino correspond to pins at player, RX must be protected by 1k resistence
// KY-040 Rotary Encoder

SoundService::SoundService() : playerSerial(PLAYER_RX, PLAYER_TX), volumeEncoder(VOLUME_DT, VOLUME_CLK) {
}

void SoundService::setup() {
  PeriphericalService::setup(SOUND_MODULE_ID);
  pinMode(PLAYER_BUSY, INPUT);
  pinMode(PLAYER_LED, OUTPUT);
  pinMode(VOLUME_CLK, INPUT);
  pinMode(VOLUME_DT, INPUT);
  pinMode(VOLUME_SW, INPUT_PULLUP);
  volume = INITIAL_VOLUME;
  setupPlayer();
}

void SoundService::loop() {
  PeriphericalService::loop();
  playerState = checkPlayer() ? PlayerState::PLAYING : PlayerState::IDLE;
  digitalWrite(PLAYER_LED, playerState == PlayerState::PLAYING ? HIGH : LOW);
  updatePlayerVolume();
}

void SoundService::processRequest(BaseRequest* request) {
  switch (request->type) {
    case RequestType::SOUND_PLAY:
      PlaySoundRequest* playRequest = (PlaySoundRequest*)request;
      play(playRequest->folder, playRequest->track);
      break;
    case RequestType::SOUND_STOP:
      stop();
      break;
  }
}

void SoundService::publishStatus() {
  SoundStatus status;
  status.playerState = playerState;
  sendStatus(&status, sizeof(SoundStatus));
}

void SoundService::setupPlayer() {
  LOG("Initialize player...");
  playerSerial.begin(9600);
  if (!player.begin(playerSerial, true, false)) {
    LOG("Error: initializing player failed.");
    activityControls.blink(PLAYER_LED, 50, 50);
  }
  else {
    LOG("Player online.");
    activityControls.blink(PLAYER_LED, 4, 250);
  }
  LOG("try disable loop...");
  player.disableLoop();

  LOG("try set volume...");
  player.volume(volume);

  LOG("try read volume...");
  int volume = player.readVolume();
  LOG("volume : %d", volume);
  // player.setTimeOut(100);
  // player.EQ(DFPLAYER_EQ_NORMAL);
  // player.outputDevice(DFPLAYER_DEVICE_SD);
}

bool SoundService::checkPlayer() {
  bool playing = digitalRead(PLAYER_BUSY) == LOW;
  // LOG("playing: %d", playing);
  return playing;
}

void SoundService::play(int folder, int track) {
  LOG("play folder: folder=%d, track=%d", folder, track);
  player.playFolder(folder, track);
  awaitPlayerBusy(true);
}

void SoundService::stop() {
  player.stop();
  awaitPlayerBusy(false);
}

void SoundService::updatePlayerVolume() {
  bool updateVolume = false;
  long position = volumeEncoder.read();

  if (position != volumeEncoderPosition) {
    if (mute) {
      mute = false;
    }
    else {
      if (position > volumeEncoderPosition) {
        volume = min(volume + 1, MAXIMUM_VOLUME);
      }
      else {
        volume = max(volume - 1, 0);
      }
    }
    updateVolume = true;
    volumeEncoderPosition = position;
  }
  else if (activityControls.buttonPressed(VOLUME_SW)) {
    LOG("volume button pressed");
    mute = !mute;
    updateVolume = true;
  }

  if (updateVolume) {
    LOG("set volume: mute=%d, volumne:%d", mute, volume);
    player.volume(mute ? 0 : volume);
  }
}

void SoundService::awaitPlayerBusy(bool busy) {
  for (int idx = 0; idx < WAIT_FOR_BUSY_COUNT; idx++) {
    bool playing = checkPlayer();
    // LOG("await player to become busy: required=%d, current=%d, cycle=%d", busy, playing, idx);
    if (busy && playing || !busy && !playing) {
      break;
    }
    // LOG("wait for player: cycle=%d", busy, idx);
    delay(WAIT_FOR_BUSY_TIMEOUT);
  }
}
