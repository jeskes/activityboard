#include "dicegadget.h"

#include <Wire.h>
#include <ablib.h>

const MessageId MESSAGE_WELCOME = "WELCOME";
const MessageId MESSAGE_GUESS = "GUESS";
const MessageId MESSAGE_STARTED = "STARTED";
const MessageId MESSAGE_NUMBER = "NUMBER";
const MessageId MESSAGE_MATCH = "MATCH";
const MessageId MESSAGE_SORRY = "SORRY";

const BitmapId BITMAP_STARTUP = "STARTUP";

const ParameterId PARAMETER_NUMBER = "number";
const ParameterId PARAMETER_GUESS = "guess";

#define BLINK_RATE 50

#define NOISE_PIN A0
#define START_BUTTON 10

#define INDICATING_TIMEOUT 1000
#define JINGLE_PLAYING_TIMEOUT 12000
#define NUMBER_PLAYING_TIMEOUT 15000
#define AUTORESET_TIMEOUT 20000

const uint8_t indicators[] = {2, 3, 4, 5, 6, 7};
const uint8_t countIndicators = sizeof(indicators) / sizeof(uint8_t);

DiceGadget::DiceGadget()
    : sound(DICEGADGET_ID, scheduler),
      rotary(scheduler),
      display(DICEGADGET_ID, scheduler) {
}

void DiceGadget::setup() {
  GadgetBase::setup();

  numpad.setup();
  rotary.setup(this);

  randomSeed(analogRead(NOISE_PIN));
  pinMode(START_BUTTON, INPUT_PULLUP);
  for (int idx = 0; idx < countIndicators; idx++) {
    pinMode(indicators[idx], OUTPUT);
  }

  activityControls.blink(indicators, countIndicators, 2, 200);

  display.displayBitmap(BITMAP_STARTUP);

  LOG("Dice Gadget started.");
}

void DiceGadget::loop() {
  GadgetBase::loop();
  stateMachine();
  showIndicators();
}

void DiceGadget::stateMachine() {
  switch (status) {
    case Status::IDLE:
      if (pendingGuessDisplay && (millis() > wireBlock)) {
        pendingGuessDisplay = 0;
        display.putParam(PARAMETER_GUESS, currentGuess);
        display.displayMessage(MESSAGE_GUESS);
      }
      if (checkNumpad() || checkStartButton()) {
        status = Status::START_REQUESTED;
      }
      break;
    case Status::START_REQUESTED:
      throwDice();
      status = Status::JINGLE_SOUND_PLAYING;
      break;
    case Status::JINGLE_SOUND_PLAYING:
      activityControls.blink(indicators, countIndicators, 1, BLINK_RATE);
      break;
    case Status::JINGLE_SOUND_ENDED:
      status = Status::NUMBER_SOUND_PLAYING;
      sound.play(10 * currentLanguage + currentNumber, this);
      display.displayMessage(MESSAGE_NUMBER);
      break;
    case Status::NUMBER_SOUND_ENDED:
      status = Status::NUMBER_INDICATING;
      startTimeIndicator = millis();
      if (currentGuess) {
        sound.play(200 + (currentGuess == currentNumber ? 1 : 2), nullptr);
        display.displayMessage(currentGuess == currentNumber ? MESSAGE_MATCH : MESSAGE_SORRY);
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
  if ((status > Status::START_REQUESTED) && (millis() > startTime + AUTORESET_TIMEOUT)) {
    LOG("auto-reset");
    reset();
  }
}

bool DiceGadget::checkNumpad() {
  char ch = numpad.getChar();
  if (ch) {
    LOG("numpad key: %c", ch);
    int guess = ch - (int)'0';
    if (guess >= 1 && guess <= 6) {
      setGuess(guess);
      return true;
    }
  }
  return false;
}

bool DiceGadget::checkStartButton() {
  bool pressed = activityControls.buttonPressed(START_BUTTON);
  if (pressed) {
    LOG("pressed");
  }
  return pressed;
}

void DiceGadget::setGuess(int guess) {
  LOG("set guess: %d", guess);
  if (currentGuess != guess) {
    currentGuess = guess;
    pendingGuessDisplay = true;
  }
}

void DiceGadget::throwDice() {
  startTime = millis();
  currentNumber = random(1, countIndicators + 1);
  currentLanguage = random(0, LANGUAGES_COUNT);
  sound.play(selectedJingle, this);
  display.putParam(PARAMETER_NUMBER, currentNumber);
  display.displayMessage(MESSAGE_STARTED);
}

void DiceGadget::reset() {
  status = Status::IDLE;
  currentGuess = 0;
  currentNumber = 0;
  currentLanguage = 0;
  startTime = 0;
  startTimeIndicator = 0;
  wireBlock = 0;
  pendingGuessDisplay = false;
  sound.setStatusHandler(nullptr);
  rotary.reset(ROTARY_ENCODER_RIGHT);
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

bool DiceGadget::rotaryStatusChanged(RotaryStatus rotaryStatus) {
  uint32_t jingle = rotaryStatus[ROTARY_ENCODER_LEFT].value / 4;
  selectedJingle = INITIAL_JINGLE + (abs(jingle) % JINGLES_COUNT);

  if (status == Status::IDLE) {
    int32_t num = (rotaryStatus[ROTARY_ENCODER_RIGHT].value / 4) % 6;
    setGuess(num >= 0 ? num + 1 : 7 + num);
    if (rotaryStatus[ROTARY_ENCODER_RIGHT].pressed) {
      status = Status::START_REQUESTED;
    }
    wireBlock = millis() + 200;
  }

  return false;
}
