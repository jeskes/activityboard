#include <Wire.h>
#include <ablib.h>

#define MODULE_ID 10
#define SOUND_FOLDER 1

const int buttons[] = { 4, 5, 6, 7 };
const int countButtons = sizeof(buttons) / sizeof(int);

const int indicators[] = { 8, 9, 10, 11 };
const int countIndicators = sizeof(indicators) / sizeof(int);

int currentSound = -1;

static void checkButtons();
static void indicateBusy();
static void receiveResult(MasterResult result);

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
  delay(100);
}

static void checkButtons() {
  for (int idx = 0; idx < countButtons; idx++) {
    int pressed = buttonPressed(buttons[idx]);
    if (pressed) {
      if (currentSound != -1) {
        LOG("stop sound");
        sendStopSoundRequest();
        currentSound = -1;
      } else {
        currentSound = idx;
        LOG("start sound: %d", idx);
        sendPlaySoundRequest(SOUND_FOLDER, idx+1, idx);
        blink(indicators, countIndicators, 5, 50);
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
    LOG("sound ended code=%d", result.resultCode);
    digitalWrite(result.requestContext, LOW);
    currentSound = -1;
  }
}