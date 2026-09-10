#include <Arduino.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <ablib.h>
#include "handler.h"
#include "sound.h"

#define PLAYER_RX 3
#define PLAYER_TX 2
#define PLAYER_BUSY 12
#define PLAYER_LED 9
#define VOLUME_CLK 6
#define VOLUME_DT 7
#define VOLUME_SW 8

static DFRobotDFPlayerMini player;

// pins at arduino correspond to pins at player, RX must be protected by 1k resistence
static SoftwareSerial playerSerial(PLAYER_RX, PLAYER_TX);

// KY-040 Rotary Encoder
static int lastClkState;
static int lastSwState = HIGH;
static unsigned long lastDebounceTime = 0;
static const unsigned long DEBOUNCE_DELAY = 50;

static void initializePlayer(MasterState* state);
static bool checkPlayer(MasterState* state);
static void playSound(MasterState* state, int folder, int track);
static void stopSound(MasterState* state, ResultCode code);
static void updatePlayerVolume(MasterState* state);
static void setSoundState(MasterState* state, MasterRequest* request);
static void clearSoundState(MasterState* state);
static void sendSoundResult(int moduleId, long requestId, long requestContext, RequestType requestType, ResultCode resultCode);

void initializeSound(MasterState* state) {
  pinMode(PLAYER_BUSY, INPUT);
  pinMode(PLAYER_LED, OUTPUT);
  pinMode(VOLUME_CLK, INPUT_PULLUP);
  pinMode(VOLUME_DT, INPUT_PULLUP);
  pinMode(VOLUME_SW, INPUT_PULLUP);
  lastClkState = digitalRead(VOLUME_CLK);
  initializePlayer(state);
}

void processSound(MasterState* state) {
  bool playing = checkPlayer(state);
  if (!playing) {
    stopSound(state, ResultCode::SUCCESS);
  }
  digitalWrite(PLAYER_LED, playing ? HIGH : LOW);
  updatePlayerVolume(state);
}

void handlePlaySoundRequest(MasterState* state, MasterRequest* request) {
  LOG("play sound request : moduleId=%d, requestId=%d, folder=%d, track=%d", request->moduleId, request->requestId, request->data.playSound.folder, request->data.playSound.track);
  stopSound(state, ResultCode::INTERRUPTED);
  setSoundState(state, request);
  playSound(state, request->data.playSound.folder, request->data.playSound.track);
}

void handleStopSoundRequest(MasterState* state, MasterRequest* request) {
  LOG("stop sound request : requestId=%d", request->requestId);
  stopSound(state, ResultCode::STOPPED);
  sendSoundResult(request->moduleId, request->requestId, request->requestContext, RequestType::STOP_SOUND, ResultCode::SUCCESS);
}

static void initializePlayer(MasterState* state) {
  LOG("Initialize player...");
  playerSerial.begin(9600);
  if (!player.begin(playerSerial, true, false)) {
    LOG("Error: initializing player failed.");
    blink(PLAYER_LED, 50, 50);
  } else {
    LOG("Player online.");
    blink(PLAYER_LED, 4, 250);
  }
  player.disableLoop();
  // player.setTimeOut(100);
  // player.EQ(DFPLAYER_EQ_NORMAL);
  // player.outputDevice(DFPLAYER_DEVICE_SD);
}

static bool checkPlayer(MasterState* state) {
  bool playing = digitalRead(PLAYER_BUSY) == LOW;
  // LOG("playing: %d", playing);
  return playing;
}

static void playSound(MasterState* state, int folder, int track) {
  LOG("play sound: folder=%d, track=%d", folder, track);
  player.playFolder(folder, track);
  delay(500);
}

static void stopSound(MasterState* state, ResultCode resultCode) {
  player.stop();
  if (state->sound.moduleId) {
    LOG("stopped sound for module=%d", state->sound.moduleId);
    sendSoundResult(
      state->sound.moduleId,
      state->sound.requestId,
      state->sound.requestContext,
      RequestType::PLAY_SOUND,
      resultCode);
    clearSoundState(state);
  }
}

static void sendSoundResult(int moduleId, long requestId, long requestContext, RequestType requestType, ResultCode resultCode) {
  MasterResult result;
  result.moduleId = moduleId;
  result.requestId = requestId;
  result.requestContext = requestContext;
  result.requestType = requestType;
  result.resultCode = resultCode;
  sendMasterResult(&result);
}

static void updatePlayerVolume(MasterState* state) {
  bool updateVolume = false;
  int currentClkState = digitalRead(VOLUME_CLK);
  if (currentClkState != lastClkState && currentClkState == LOW) {
    if (state->sound.mute) {
      state->sound.mute = false;
      updateVolume = true;
    }
    if (digitalRead(VOLUME_DT) != currentClkState) {
      if (state->sound.volume < 30) {
        state->sound.volume++;
        updateVolume = true;
      }
    } else {
      if (state->sound.volume > 0) {
        state->sound.volume--;
        updateVolume = true;
      }
    }
  }
  lastClkState = currentClkState;

  int currentSwState = digitalRead(VOLUME_SW);
  if (currentSwState != lastSwState) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY && currentSwState == LOW) {
    if (!state->sound.mute) {
      state->sound.mute = true;
      updateVolume = true;
    } else {
      state->sound.mute = false;
      updateVolume = true;
    }
    while (digitalRead(VOLUME_SW) == LOW)
      ;
  }
  lastSwState = currentSwState;

  if (updateVolume) {
    player.volume(state->sound.mute ? 0 : state->sound.volume);
  }
}

static void setSoundState(MasterState* state, MasterRequest* request) {
  state->sound.moduleId = request->moduleId;
  state->sound.requestId = request->requestId;
  state->sound.requestContext = request->requestContext;
  state->sound.started = millis();
}

static void clearSoundState(MasterState* state) {
  state->sound.moduleId = 0;
  state->sound.requestId = 0;
  state->sound.requestContext = 0;
  state->sound.started = 0;
}
