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

#define INITIAL_FOLDER_ID 10
#define STARTUP_TRACK_ID 100

static long volumeEncoderPosition = -999;

// pins at arduino correspond to pins at player, RX must be protected by 1k resistence
// KY-040 Rotary Encoder

SoundService::SoundService()
    : currentVolume(INITIAL_VOLUME),
      pendingVolume(INITIAL_VOLUME),
      folderId(INITIAL_FOLDER_ID),
      volumeEncoder(VOLUME_DT, VOLUME_CLK),
      playerSerial(PLAYER_RX, PLAYER_TX) {
}

void SoundService::setup(uint8_t module) {
  PeriphericalService::setup(module);
  pinMode(PLAYER_BUSY, INPUT);
  pinMode(VOLUME_CLK, INPUT_PULLUP);
  pinMode(VOLUME_DT, INPUT_PULLUP);
  pinMode(VOLUME_SW, INPUT_PULLUP);
  pinMode(PLAYER_LED, OUTPUT);
  setupPlayer();
}

void SoundService::loop() {
  PeriphericalService::loop();
  playerState = checkPlayer() ? PlayerState::PLAYING : PlayerState::IDLE;
  digitalWrite(PLAYER_LED, playerState == PlayerState::PLAYING ? HIGH : LOW);
  readPlayerVolume();
  updatePlayerVolume();
}

void SoundService::processRequest(BaseRequest* request) {
  switch (request->type) {
    case RequestType::SOUND_SETUP:
      clientSetup(((SetupSoundRequest*)request)->folderId);
      break;
    case RequestType::SOUND_PLAY:
      play(((PlaySoundRequest*)request)->trackId);
      break;
    case RequestType::SOUND_STOP:
      stop();
      break;
    default:
      break;
  }
}

void SoundService::publishStatus() {
  SoundStatus status;
  status.playerState = playerState;
  // LOG("publish status");
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
    activityControls.blink(PLAYER_LED, 4, 250);
  }
  player.disableLoop();
  player.volume(pendingVolume);
  // player.setTimeOut(100);
  // player.EQ(DFPLAYER_EQ_NORMAL);
  // player.outputDevice(DFPLAYER_DEVICE_SD);

  play(STARTUP_TRACK_ID);
}

bool SoundService::checkPlayer() {
  bool playing = digitalRead(PLAYER_BUSY) == LOW;
  // LOG("playing: %d", playing);
  return playing;
}

void SoundService::clientSetup(uint16_t folderId) {
  LOG("client setup sound: folder=%d", folderId);
  this->folderId = folderId;
  playerState = PlayerState::IDLE;
}

void SoundService::play(uint16_t trackId) {
  LOG("play sound: track=%d", trackId);
  playerState = PlayerState::PLAYING;
  player.playFolder(folderId, trackId);
  awaitPlayerBusy(true);
}

void SoundService::stop() {
  player.stop();
  awaitPlayerBusy(false);
}

void SoundService::readPlayerVolume() {
  long position = volumeEncoder.read();
  if (position != volumeEncoderPosition) {
    if (currentMute) {
      pendingMute = false;
    }
    else {
      if (position > volumeEncoderPosition) {
        pendingVolume = min(currentVolume + 1, MAXIMUM_VOLUME);
      }
      else {
        pendingVolume = max(currentVolume - 1, 0);
      }
    }
    volumeEncoderPosition = position;
  }
  else if (activityControls.buttonPressed(VOLUME_SW)) {
    LOG("volume button pressed");
    pendingMute = !currentMute;
  }
}

static unsigned long volumeUpdateRate = 200;
static unsigned long lastVolumeUpdate = 0;

void SoundService::updatePlayerVolume() {
  if (millis() > lastVolumeUpdate + volumeUpdateRate) {
    lastVolumeUpdate = millis();
    // LOG("mute [%d,%d], volume [%d,%d]", currentMute, pendingMute, currentVolume, pendingVolume);
    if (pendingMute != currentMute || pendingVolume != currentVolume) {
      currentMute = pendingMute;
      currentVolume = pendingVolume;
      LOG(" set volume mute=%d, volume=%d", currentMute, currentVolume);
      player.volume(currentMute ? 0 : currentVolume);
    }
  }
}

void SoundService::awaitPlayerBusy(bool busy) {
  for (int idx = 0; idx < WAIT_FOR_BUSY_COUNT; idx++) {
    bool playing = checkPlayer();
    // LOG("await player to become busy: required=%d, current=%d, cycle=%d", busy, playing, idx);
    if ((busy && playing) || (!busy && !playing)) {
      break;
    }
    // LOG("wait for player: cycle=%d", busy, idx);
    delay(WAIT_FOR_BUSY_TIMEOUT);
  }
}
