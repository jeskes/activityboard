#include <Wire.h>
#include <ablib.h>

#define MODULE_ID 10
#define SOUND_FOLDER 10

const int buttons[] = { 4, 5, 6, 7 };
const int countButtons = sizeof(buttons) / sizeof(int);

const int indicators[] = { 8, 9, 10, 11 };
const int countIndicators = sizeof(indicators) / sizeof(int);

int currentSound = -1;

static void checkButtons();
static void indicateBusy();
static void receiveResult(MasterResult result);
static void processPlaySoundResult(MasterResult result);

void setup() {
  initActivityBoardLibrary(MODULE_ID, receiveResult);

  for (int idx = 0; idx < countButtons; idx++) {
    pinMode(buttons[idx], INPUT_PULLUP);
  }
  for (int idx = 0; idx < countIndicators; idx++) {
    pinMode(indicators[idx], OUTPUT);
  }

  blink(indicators, countIndicators, 1, 500);
  LOG("Sound Gadget started - module id : %d", MODULE_ID);
}

void loop() {
  checkButtons();
  indicateBusy();
}

static void checkButtons() {
  for (int idx = 0; idx < countButtons; idx++) {
    int pressed = buttonPressed(buttons[idx]);
    if (pressed) {
      if (currentSound != -1) {
        LOG("stop sound: %d", currentSound);
        sendStopSoundRequest();
        currentSound = -1;
      } else {
        currentSound = idx;
        LOG("start sound: %d", idx);
        blink(indicators, countIndicators, 5, 50);
        sendPlaySoundRequest(SOUND_FOLDER, idx + 1, idx);
      }
      break;
    }
  }
}

static void indicateBusy() {
  for (int idx = 0; idx < countIndicators; idx++) {
    int level = (idx == currentSound) ? HIGH : LOW;
    digitalWrite(indicators[idx], level);
  }
}

static void receiveResult(MasterResult result) {
  if (result.requestType == RequestType::PLAY_SOUND) {
    processPlaySoundResult(result);
  }
}

static void processPlaySoundResult(MasterResult result) {
  LOG("process play-sound result: code=%d, context=%d", result.resultCode, result.requestContext);
  switch (result.resultCode) {
    case ResultCode::SUCCESS:
    case ResultCode::STOPPED:
    case ResultCode::PAUSED:
    case ResultCode::INTERRUPTED:
    case ResultCode::FAILED:
      currentSound = -1;
      break;
    case ResultCode::STARTED:
    case ResultCode::RESUMED:
      currentSound = result.requestContext;
      break;
  }
}