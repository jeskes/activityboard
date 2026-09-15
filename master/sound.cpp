#include <Arduino.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <Encoder.h>
#include <ablib.h>
#include "handler.h"
#include "sound.h"

// pins

#define VOLUME_CLK 2
#define VOLUME_DT 3
#define VOLUME_SW 4
#define PLAYER_LED 9
#define PLAYER_TX 10
#define PLAYER_RX 11
#define PLAYER_BUSY 12

#define INITIAL_VOLUME 5
#define MAXIMUM_VOLUME 25

#define WAIT_FOR_BUSY_COUNT 5
#define WAIT_FOR_BUSY_TIMEOUT 300

static DFRobotDFPlayerMini player;

// pins at arduino correspond to pins at player, RX must be protected by 1k resistence
static SoftwareSerial playerSerial(PLAYER_RX, PLAYER_TX);

// KY-040 Rotary Encoder
Encoder volumeEncoder(VOLUME_DT, VOLUME_CLK);
long volumeEncoderPosition = -999;

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
  pinMode(VOLUME_CLK, INPUT);
  pinMode(VOLUME_DT, INPUT);
  pinMode(VOLUME_SW, INPUT_PULLUP);
  state->sound.volume = INITIAL_VOLUME;
  initializePlayer(state);
}

void processSound(MasterState* state) {
  bool playing = checkPlayer(state);
  // LOG("playing : %d", playing);
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
  LOG("try disable loop...");
  player.disableLoop();

  LOG("try set volume...");
  player.volume(state->sound.volume);

  LOG("try read volume...");
  int volume = player.readVolume();
  LOG("volume : %d", volume);
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
  player.playFolder(folder, track);
  for (int idx = 0; idx < WAIT_FOR_BUSY_COUNT; idx++) {
    delay(WAIT_FOR_BUSY_TIMEOUT);
    if (checkPlayer(state)) {
      break;
    }
  }

  if (state->sound.moduleId) {
    LOG("play sound for module=%d: folder=%d, track=%d", state->sound.moduleId, folder, track);
    sendSoundResult(
      state->sound.moduleId,
      state->sound.requestId,
      state->sound.requestContext,
      RequestType::PLAY_SOUND,
      checkPlayer(state) ? ResultCode::STARTED : ResultCode::FAILED);
  }
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
  LOG("send result: module=%d, type=%d, context=%ld, code=%d", moduleId, requestType, requestContext, resultCode);
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
  long position = volumeEncoder.read();

  if (position != volumeEncoderPosition) {
    if (state->sound.mute) {
      state->sound.mute = false;
    } else {
      if (position > volumeEncoderPosition) {
        state->sound.volume = min(state->sound.volume + 1, MAXIMUM_VOLUME);
      } else {
        state->sound.volume = max(state->sound.volume - 1, 0);
      }
    }
    updateVolume = true;
    volumeEncoderPosition = position;

  } else if (buttonPressed(VOLUME_SW)) {
    LOG("volume button pressed");
    state->sound.mute = !state->sound.mute;
    updateVolume = true;
  }

  if (updateVolume) {
    LOG("set volume: mute=%d, volumne:%d", state->sound.mute, state->sound.volume);
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
