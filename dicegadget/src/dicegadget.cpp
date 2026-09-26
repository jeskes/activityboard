#include "dicegadget.h"

#include <Wire.h>
#include <ablib.h>

#define SOUND_FOLDER 11

/* languages: 00x english, 01x bavarian, 02x russian, 03x german */
#define LANGUAGES_COUNT 4

/* jingle sounds: 101 ... 110 */
#define JINGLES_COUNT 10

#define BLINK_RATE 50

#define NOISE_PIN A0
#define START_BUTTON 10
#define BUZZER 12

#define AUTORESET_TIMEOUT 30000
#define INDICATING_TIMEOUT 1000

const int indicators[] = {2, 3, 4, 5, 6, 7};
const int countIndicators = sizeof(indicators) / sizeof(int);

DiceGadget::DiceGadget() : sound(&this->scheduler) {}

void DiceGadget::setup() {
  GadgetBase::setup();

  randomSeed(analogRead(NOISE_PIN));
  pinMode(START_BUTTON, INPUT_PULLUP);
  for (int idx = 0; idx < countIndicators; idx++) {
    pinMode(indicators[idx], OUTPUT);
  }
  activityControls.blink(indicators, countIndicators, 2, 200);

  LOG("Dice Gadget started.");
}

void DiceGadget::loop() {
  GadgetBase::loop();
  stateMachine();
  showIndicators();
}

void DiceGadget::stateMachine() {
  int pressed = activityControls.buttonPressed(START_BUTTON);
  if (pressed && status != Status::IDLE) {
    tone(BUZZER, 150);
    delay(200);
    noTone(BUZZER);
  }

  switch (status) {
    case Status::IDLE:
      if (pressed) {
        startTime = millis();
        currentNumber = random(1, countIndicators + 1);
        currentLanguage = random(0, LANGUAGES_COUNT);
        LOG("current number: %d, language: %d", currentNumber, currentLanguage);
        status = Status::JINGLE_SOUND_PLAYING;
        int jingle = 100 + random(1, JINGLES_COUNT + 1);
        sound.play(SOUND_FOLDER, jingle, this);
      }
      break;
    case Status::JINGLE_SOUND_PLAYING:
      activityControls.blink(indicators, countIndicators, 1, BLINK_RATE);
      break;
    case Status::JINGLE_SOUND_ENDED:
      status = Status::NUMBER_SOUND_PLAYING;
      sound.play(SOUND_FOLDER, 10 * currentLanguage + currentNumber, this);
      break;
    case Status::NUMBER_SOUND_ENDED:
      status = Status::NUMBER_INDICATING;
      startTimeIndicator = millis();
      break;
    case Status::NUMBER_INDICATING:
      if (millis() > startTimeIndicator + INDICATING_TIMEOUT) {
        reset();
      }
  }

  if ((status != Status::IDLE) && (millis() > startTime + AUTORESET_TIMEOUT)) {
    LOG("auto-reset");
    reset();
  }
}

void DiceGadget::reset() {
  status = Status::IDLE;
  currentNumber = 0;
  currentLanguage = 0;
  startTime = 0;
  startTimeIndicator = 0;
}

void DiceGadget::showIndicators() {
  for (int idx = 0; idx < countIndicators; idx++) {
    int level = (idx == currentNumber - 1) ? HIGH : LOW;
    digitalWrite(indicators[idx], level);
  }
}

bool DiceGadget::playerStateChanged(PlayerState playerState) {
  if (playerState == PlayerState::IDLE) {
    switch (status) {
      case Status::JINGLE_SOUND_PLAYING:
        status = Status::JINGLE_SOUND_ENDED;
        return true;
      case Status::NUMBER_SOUND_PLAYING:
        status = Status::NUMBER_SOUND_ENDED;
        return true;
    }
  }
  return false;
}
