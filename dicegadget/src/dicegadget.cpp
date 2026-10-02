#include "dicegadget.h"

#include <Wire.h>
#include <ablib.h>

#define MESSAGE_WELCOME "WELCOME"
#define MESSAGE_STARTED "STARTED"

/* languages: 00x english, 01x bavarian, 02x russian, 03x german */
#define LANGUAGES_COUNT 4

/* jingle sounds: 101 ... 110 */
#define JINGLES_COUNT 10

#define BLINK_RATE 50

#define NOISE_PIN A0
#define START_BUTTON 10
#define BUZZER 12

#define INDICATING_TIMEOUT 1000
#define JINGLE_PLAYING_TIMEOUT 12000
#define NUMBER_PLAYING_TIMEOUT 15000
#define AUTORESET_TIMEOUT 20000

const uint8_t indicators[] = {2, 3, 4, 5, 6, 7};
const uint8_t countIndicators = sizeof(indicators) / sizeof(uint8_t);

DiceGadget::DiceGadget()
    : sound(&this->scheduler) {
}

void DiceGadget::setup() {
  GadgetBase::setup();

  sound.init(DICEGADGET_ID);
  display.init(DICEGADGET_ID);
  numpad.init(DICEGADGET_ID);

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

  char num = numpad.getChar();
  if (num) {
    if (status == Status::IDLE) {
      int d = num - (int)'0';
      if (d >= 1 && d <= 6) {
        currentGuess = d;
      }
    }
    else {
      tone(BUZZER, 150);
      delay(200);
      noTone(BUZZER);
    }
  }

  switch (status) {
    case Status::IDLE:
      if (pressed || currentGuess) {
        startTime = millis();
        currentNumber = random(1, countIndicators + 1);
        currentLanguage = random(0, LANGUAGES_COUNT);
        status = Status::JINGLE_SOUND_PLAYING;
        uint16_t jingle = 100 + random(1, JINGLES_COUNT + 1);
        sound.play(jingle, this);
        display.displayMessage(MESSAGE_STARTED);
      }
      break;
    case Status::JINGLE_SOUND_PLAYING:
      activityControls.blink(indicators, countIndicators, 1, BLINK_RATE);
      break;
    case Status::JINGLE_SOUND_ENDED:
      status = Status::NUMBER_SOUND_PLAYING;
      sound.play(10 * currentLanguage + currentNumber, this);
      break;
    case Status::NUMBER_SOUND_ENDED:
      status = Status::NUMBER_INDICATING;
      startTimeIndicator = millis();
      LOG("current number: %d, current guess: %d", currentNumber, currentGuess);
      if (currentGuess) {
        sound.play(200 + (currentGuess == currentNumber ? 1 : 2), nullptr);
      }
      break;
    case Status::NUMBER_INDICATING:
      if (millis() > startTimeIndicator + INDICATING_TIMEOUT) {
        reset();
      }
    default:
      break;
  }

  if ((status == Status::JINGLE_SOUND_PLAYING) && (millis() > startTime + JINGLE_PLAYING_TIMEOUT)) {
    LOG("jingle-playing-reset");
    status = Status::JINGLE_SOUND_ENDED;
  }
  if ((status == Status::NUMBER_SOUND_PLAYING) && (millis() > startTime + NUMBER_PLAYING_TIMEOUT)) {
    LOG("number-playing-reset");
    status = Status::NUMBER_SOUND_ENDED;
  }
  if ((status != Status::IDLE) && (millis() > startTime + AUTORESET_TIMEOUT)) {
    LOG("auto-reset");
    reset();
  }
}

void DiceGadget::reset() {
  status = Status::IDLE;
  currentGuess = 0;
  currentNumber = 0;
  currentLanguage = 0;
  startTime = 0;
  startTimeIndicator = 0;
  sound.setStatusHandler(nullptr);
}

void DiceGadget::showIndicators() {
  for (int idx = 0; idx < countIndicators; idx++) {
    int level = (idx == currentNumber - 1) ? HIGH : LOW;
    digitalWrite(indicators[idx], level);
  }
}

bool DiceGadget::playerStateChanged(PlayerState playerState) {
  // LOG("dice-status=%d, player-state=%d", status, playerState);
  if (playerState == PlayerState::IDLE) {
    switch (status) {
      case Status::JINGLE_SOUND_PLAYING:
        status = Status::JINGLE_SOUND_ENDED;
        return true;
      case Status::NUMBER_SOUND_PLAYING:
        status = Status::NUMBER_SOUND_ENDED;
        return true;
      default:
        break;
    }
  }
  return false;
}
