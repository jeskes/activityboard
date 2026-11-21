#include "soundcontroller.h"

#include "../../sound/src/soundcom.h"
#include "const.h"

static long volumeEncoderPosition = -999;
static unsigned long volumeEncoderDebounceTime = 0;

SoundController::SoundController(LedRing& ledRing, RotaryExpansion& rotary)
    : ledRing(ledRing),
      volumeAdapter(INITIAL_VOLUME, MINIMUM_VOLUME, MAXIMUM_VOLUME, VOLUMNE_STEP),
      uartController(Serial2),
      mute(false),
      volume(INITIAL_VOLUME),
      playerState(PlayerState::UNKNOWN),
      debouncePlaying(false),
      debounceTimestamp(0) {
  rotary.setAdapterLeft(volumeAdapter);
}

void SoundController::setup() {
  LOG("Initialize sound system: rx=%d, tx=%d, busy=%d", SOUND_SYSTEM_RX, SOUND_SYSTEM_TX, SOUND_SYSTEM_BUSY);

  uartController.setup(SOUND_SYSTEM_TX, SOUND_SYSTEM_RX);
  uartController.setHandler(this);

  pinMode(SOUND_SYSTEM_BUSY, INPUT_PULLUP);

  volumeAdapter.onPressed([&]() { toggleMute(); });
  volumeAdapter.onValueChanged([&](long volume) { setVolume(volume); });

  delay(100);

  setVolume(volume);
  ledRing.updateVolume(volume, mute);
  LOG("successfully initialized player");
}

void SoundController::onPlaying(SoundPlayingHandler handler) {
  playingHandler = handler;
}

void SoundController::onStateChange(SoundStateChangeHandler handler) {
  stateChangeHandler = handler;
}

void SoundController::loop() {
  bool playing = checkPlaying();
  if (playing != debouncePlaying || !debounceTimestamp) {
    debouncePlaying = playing;
    debounceTimestamp = millis();
  }
  else if (millis() > debounceTimestamp + PLAYER_DEBOUNCE_DELAY) {
    PlayerState oldState = playerState;
    playerState = playing ? PlayerState::PLAYING : PlayerState::IDLE;
    if (stateChangeHandler && oldState != playerState) {
      stateChangeHandler();
    }
  }

  uartController.loop();
}

void SoundController::startMaintenance() {
  uartController.send(SoundCom::START_MAINTENANCE);
}

void SoundController::stopMaintenance() {
  uartController.send(SoundCom::STOP_MAINTENANCE);
}

bool SoundController::checkPlaying() {
  return digitalRead(SOUND_SYSTEM_BUSY) == LOW;
}

void SoundController::play(TrackPath path) {
  LOG("play sound: path=%s", path);
  playerState = PlayerState::UNKNOWN;
  uartController.send(SoundCom::PLAY, path);
  awaitBusy(true);
}

void SoundController::stop() {
  uartController.send(SoundCom::STOP);
  awaitBusy(false);
}

void SoundController::setVolume(int value) {
  mute = false;
  volume = min(max(value, (int)MINIMUM_VOLUME), (int)MAXIMUM_VOLUME);
  LOG("set volume: vol=%d", volume);
  char buffer[10];
  uartController.send(SoundCom::VOLUME, itoa(volume, buffer, 10));
  ledRing.updateVolume(volume, mute);
}

void SoundController::toggleMute() {
  mute = !mute;
  LOG("toggle mute: mute=%d", (int)mute);
  char buffer[10];
  uartController.send(SoundCom::VOLUME, itoa(mute ? 0 : volume, buffer, 10));
  ledRing.updateVolume(volume, mute);
}

void SoundController::awaitBusy(bool busy) {
  int loop = 0;
  bool playing = checkPlaying();
  while (loop < PLAYER_WAIT_FOR_BUSY_COUNT && busy != playing) {
    delay(PLAYER_WAIT_FOR_BUSY_TIMEOUT);
    loop++;
    playing = checkPlaying();
  }
  if (busy != playing) {
    LOG("==> player waited for=%d, playing=%d, loops=%d", busy, playing, loop);
  }
}

void SoundController::handleStreamCommand(StreamCommand cmd, const char* params) {
  LOG("sound command: cmd=%s, params=%s", cmd, params);
  if (!strcmp(cmd, SoundCom::PLAYING)) {
	if(playingHandler) {
		playingHandler(params);
	}
  }
}
