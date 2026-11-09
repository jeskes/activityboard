#include "player.h"

#include "const.h"

#define WAIT_FOR_BUSY_COUNT 10
#define WAIT_FOR_BUSY_TIMEOUT 100

static long volumeEncoderPosition = -999;
static unsigned long volumeEncoderDebounceTime = 0;

Player::Player(LedRing& ledRing, RotaryExpansion& rotary)
    : ledRing(ledRing),
      volumeAdapter(INITIAL_VOLUME, MINIMUM_VOLUME, MAXIMUM_VOLUME, VOLUMNE_STEP),
      playerSerial(2),
      mute(false),
      volume(INITIAL_VOLUME),
      playerState(PlayerState::UNKNOWN) {
  rotary.setAdapterLeft(volumeAdapter);
}

void Player::setup() {
  LOG("Initialize player: rx=%d, tx=%d, busy=%d", PLAYER_RX, PLAYER_TX, PLAYER_BUSY);
  pinMode(PLAYER_BUSY, INPUT_PULLUP);

  volumeAdapter.onPressed([&]() { toggleMute(); });
  volumeAdapter.onValueChanged([&](long volume) { setVolume(volume); });

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

void Player::onStateChange(PlayerStateChangeHandler handler) {
  this->handler = handler;
}

bool tempPlaying = false;
unsigned long tempTimestamp = 0;
const unsigned long DEBOUNCE_DELAY = 200;

void Player::loop() {
  bool playing = checkPlaying();
  if (playing != tempPlaying || !tempTimestamp) {
    tempPlaying = playing;
    tempTimestamp = millis();
  }
  else if (millis() > tempTimestamp + DEBOUNCE_DELAY) {
    PlayerState oldState = playerState;
    playerState = playing ? PlayerState::PLAYING : PlayerState::IDLE;
    if (handler && oldState != playerState) {
      handler(oldState, playerState);
    }
  }
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
  stop();
  player.playFolder(folder, track);
  awaitBusy(true);
}

void Player::stop() {
  player.stop();
  awaitBusy(false);
}

void Player::setVolume(int value) {
  mute = false;
  volume = value;
  LOG("set volume: vol=%d", volume);
  player.volume(volume);
  ledRing.updateVolume(volume, mute);
}

void Player::toggleMute() {
  mute = !mute;
  LOG("toggle mute: mute=%d", (int)mute);
  player.volume(mute ? 0 : volume);
  ledRing.updateVolume(volume, mute);
}

void Player::awaitBusy(bool busy) {
  int loop = 0;
  bool playing = checkPlaying();
  while (loop < WAIT_FOR_BUSY_COUNT && busy != playing) {
    delay(WAIT_FOR_BUSY_TIMEOUT);
    loop++;
    playing = checkPlaying();
  }
  LOG("player waited for=%d, playing=%d, loops=%d", busy, playing, loop);
}
