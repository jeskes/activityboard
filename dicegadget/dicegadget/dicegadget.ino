#include <Wire.h>
#include <ablib.h>

#define MODULE_ID 11
#define SOUND_FOLDER 11
#define LANGUAGES_COUNT 4 /* 00x english, 01x bavarian, 02x russian, 03x german*/

#define NOISE_PIN A0
#define START_BUTTON 10
#define BUZZER 12

#define INDICATING_TIMEOUT 1000

const int indicators[] = { 2, 3, 4, 5, 6, 7 };
const int countIndicators = sizeof(indicators) / sizeof(int);

enum class Status : int {
  IDLE = 0,
  JINGLE_SOUND_PLAYING = 1,
  JINGLE_SOUND_ENDED = 2,
  NUMBER_SOUND_PLAYING = 3,
  NUMBER_SOUND_ENDED = 4,
  NUMBER_INDICATING = 5
};

Status status = Status::IDLE;

int currentNumber = 0;
int currentLanguage = 0;
long startTimeIndicator = 0;

static void checkStartButton();
static void indicateBusy();
static void receiveResult(MasterResult result);
static void processPlaySoundResult(MasterResult result);

void setup() {
  initActivityBoardLibrary(MODULE_ID, receiveResult);

  pinMode(START_BUTTON, INPUT_PULLUP);

  for (int idx = 0; idx < countIndicators; idx++) {
    pinMode(indicators[idx], OUTPUT);
  }

  randomSeed(analogRead(NOISE_PIN));

  blink(indicators, countIndicators, 2, 200);
  LOG("Dice Gadget started - module id : %d", MODULE_ID);
}

void loop() {
  stateMachine();
  showIndicators();
}

static void stateMachine() {
  int pressed = buttonPressed(START_BUTTON);
  if (pressed && status != Status::IDLE) {
    tone(BUZZER, 150);
    delay(200);
    noTone(BUZZER);
  }

  switch (status) {
    case Status::IDLE:
      if (pressed) {
        currentNumber = random(1, countIndicators + 1);
        currentLanguage = random(0, LANGUAGES_COUNT);
        LOG("current number: %d, language: %d", currentNumber, currentLanguage);
        status = Status::JINGLE_SOUND_PLAYING;
        sendPlaySoundRequest(SOUND_FOLDER, 9, 0);
      }
      break;
    case Status::JINGLE_SOUND_PLAYING:
      blink(indicators, countIndicators, 1, 50);
      break;
    case Status::JINGLE_SOUND_ENDED:
      status = Status::NUMBER_SOUND_PLAYING;
      sendPlaySoundRequest(SOUND_FOLDER, 10 * currentLanguage + currentNumber, currentNumber);
      break;
    case Status::NUMBER_SOUND_ENDED:
      status = Status::NUMBER_INDICATING;
      startTimeIndicator = millis();
      break;
    case Status::NUMBER_INDICATING:
      if (millis() > startTimeIndicator + INDICATING_TIMEOUT) {
        currentNumber = 0;
        currentLanguage = 0;
        status = Status::IDLE;
        startTimeIndicator = 0;
      }
  }
}

static void showIndicators() {
  for (int idx = 0; idx < countIndicators; idx++) {
    int level = (idx == currentNumber - 1) ? HIGH : LOW;
    digitalWrite(indicators[idx], level);
  }
}

static void receiveResult(MasterResult result) {
  if (result.type == RequestType::PLAY_SOUND) {
    processPlaySoundResult(result);
  }
}

static void processPlaySoundResult(MasterResult result) {
  LOG("play-sound result received: code=%d, context=%ld", result.code, result.context);
  if (result.code != ResultCode::STARTED) {
    switch (status) {
      case Status::JINGLE_SOUND_PLAYING:
        LOG("jingle sound ended");
        status = Status::JINGLE_SOUND_ENDED;
        break;
      case Status::NUMBER_SOUND_PLAYING:
        LOG("number sound ended");
        status = Status::NUMBER_SOUND_ENDED;
        break;
    }
  }
}
