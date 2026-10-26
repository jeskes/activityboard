#include "sound.h"

#define WAIT_FOR_BUSY_COUNT 5
#define WAIT_FOR_BUSY_TIMEOUT 300

static long volumeEncoderPosition = -999;

PlayerState Sound::playerState = PlayerState::UNKNOWN;

Sound::Sound()
    : PeriphericalService(publishStatus),
      currentVolume(INITIAL_VOLUME),
      pendingVolume(INITIAL_VOLUME),
      volumeEncoder(VOLUME_DT, VOLUME_CLK),
      playerSerial(PLAYER_RX, PLAYER_TX) {
}

void SoundService::setup(uint8_t module) {
  PeriphericalService::setup(module);

  pinMode(PLAYER_BUSY, INPUT);
  pinMode(VOLUME_CLK, INPUT_PULLUP);
  pinMode(VOLUME_DT, INPUT_PULLUP);
  pinMode(VOLUME_SW, INPUT_PULLUP);
  pinMode(SPEAKER_CONTROL, OUTPUT);

  delay(1000);
  setupPlayer();
}

void SoundService::loop() {
  PeriphericalService::loop();
  playerState = checkPlayer() ? PlayerState::PLAYING : PlayerState::IDLE;
  // digitalWrite(PLAYER_LED, playerState == PlayerState::PLAYING ? HIGH : LOW);
  readPlayerVolume();
  updatePlayerVolume();
}

void SoundService::processRequest(BaseRequest* request) {
  switch (request->type) {
    case RequestType::SOUND_PLAY:
      play(((PlaySoundRequest*)request)->client, ((PlaySoundRequest*)request)->track);
      break;
    case RequestType::SOUND_STOP:
      stop();
      break;
    default:
      break;
  }
}

void SoundService::publishStatus() {
  SoundStatus status(playerState);
  // LOG("SoundService: publish status: player-state=%d", playerState);
  sendStatus(&status, sizeof(SoundStatus));
}

void SoundService::setupPlayer() {
  LOG("Initialize player...");
  playerSerial.begin(9600);
  if (!player.begin(playerSerial, true, false)) {
    LOG("Error: initializing player failed.");
    // activityControls.blink(PLAYER_LED, 50, 50);
  }
  else {
    // activityControls.blink(PLAYER_LED, 4, 250);
  }
  player.disableLoop();
  player.volume(pendingVolume);
  // player.setTimeOut(100);
  // player.EQ(DFPLAYER_EQ_NORMAL);
  // player.outputDevice(DFPLAYER_DEVICE_SD);
}

bool SoundService::checkPlayer() {
  bool playing = digitalRead(PLAYER_BUSY) == LOW;
  // LOG("playing: %d", playing);
  if (!playing) {
    player.sleep();
  }
  return playing;
}

void SoundService::play(ClientId client, TrackId track) {
  LOG("play sound: client=%d, track=%d", client, track);
  playerState = PlayerState::PLAYING;
  player.volume(currentVolume); // wake up
  delay(100);
  player.playFolder(client, track);
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
      LOG("set volume mute=%d, volume=%d", currentMute, currentVolume);
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
