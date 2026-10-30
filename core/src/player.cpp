#include "player.h"

#include "const.h"

#define WAIT_FOR_BUSY_COUNT 10
#define WAIT_FOR_BUSY_TIMEOUT 100

static long volumeEncoderPosition = -999;
static unsigned long volumeEncoderDebounceTime = 0;

PlayerState Player::playerState = PlayerState::UNKNOWN;

Player::Player(LedRing& ledRing)
    : ledRing(ledRing),
      playerSerial(2),
      mute(false),
      volumeEncoder(INITIAL_VOLUME, MINIMUM_VOLUME, MAXIMUM_VOLUME),
      volume(INITIAL_VOLUME) {
}

void Player::setup() {
  LOG("Initialize player: rx=%d, tx=%d, busy=%d, vol-clk=%d, vol-dt=%d, vol-sw=%d...", PLAYER_RX, PLAYER_TX, PLAYER_BUSY, VOLUME_CLK, VOLUME_DT, VOLUME_SW);
  pinMode(PLAYER_BUSY, INPUT_PULLUP);

  volumeEncoder.setup(VOLUME_CLK, VOLUME_DT, VOLUME_SW);
  volumeEncoder.setHandler([this](long volume) { this->setVolume(volume); });

  delay(100);

  playerSerial.begin(9600, SERIAL_8N1, PLAYER_TX, PLAYER_RX);
  if (!player.begin(playerSerial)) {
    LOG("initializing player failed.");
  }
  else {
    player.disableLoop();
    player.volume(volume);
    ledRing.updateVolume(volume, mute);
    // player.setTimeOut(100);
    // player.EQ(DFPLAYER_EQ_NORMAL);
    // player.outputDevice(DFPLAYER_DEVICE_SD);
    LOG("successfully initialized player: vol=%d.", player.readVolume());
  }
}

void Player::loop() {
  volumeEncoder.loop();
  if (volumeEncoder.pressed()) {
    toggleMute();
  }
  playerState = checkPlaying() ? PlayerState::PLAYING : PlayerState::IDLE;
}

bool Player::checkPlaying() {
  bool playing = digitalRead(PLAYER_BUSY) == LOW;
  if (!playing) {
    // player.sleep();
  }
  return playing;
}

void Player::play(TrackFolder folder, TrackId track) {
  LOG("play sound: folder=%d, track=%d", folder, track);
  playerState = PlayerState::UNKNOWN;
  player.playFolder(folder, track);
  awaitBusy(true);
}

void Player::stop() {
  playerState = PlayerState::UNKNOWN;
  player.stop();
  awaitBusy(false);
}

void Player::setVolume(int volume) {
  LOG("set volume: vol=%d", volume);
  mute = false;
  player.volume(volume);
  this->volume = volume;
  ledRing.updateVolume(volume, mute);
}

void Player::toggleMute() {
  mute = !mute;
  LOG("toggle mute: mute=%d", (int)mute);
  player.volume(mute ? 0 : volume);
  ledRing.updateVolume(volume, mute);
}

void Player::awaitBusy(bool busy) {
  for (int idx = 0; idx < WAIT_FOR_BUSY_COUNT; idx++) {
    bool playing = checkPlaying();
    if ((busy && playing) || (!busy && !playing)) {
      break;
    }
    delay(WAIT_FOR_BUSY_TIMEOUT);
  }
  playerState = busy ? PlayerState::PLAYING : PlayerState::IDLE;
}
