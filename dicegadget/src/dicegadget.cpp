#include "dicegadget.h"

const uint8_t indicators[] = {INDICATOR_LED_1, INDICATOR_LED_2, INDICATOR_LED_3, INDICATOR_LED_4, INDICATOR_LED_5, INDICATOR_LED_6};
const uint8_t countIndicators = sizeof(indicators) / sizeof(uint8_t);

DiceGadget::DiceGadget()
    : sound(DICEGADGET_ID, scheduler),
      rotary(scheduler),
      display(DICEGADGET_ID, scheduler) {
}

void DiceGadget::setup() {
  GadgetBase::setup();

  numpad.setup();
  rotary.setup();

  randomSeed(analogRead(NOISE_PIN));
  pinMode(START_BUTTON, INPUT_PULLUP);
  for (int idx = 0; idx < countIndicators; idx++) {
    pinMode(indicators[idx], OUTPUT);
  }

  activityControls.blink(indicators, countIndicators, 2, 200);

  display.init(BITMAP_STARTUP);
  rotary.setStatusHandler(this);

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
      if (resetDisplayTime && millis() > resetDisplayTime) {
        resetDisplayTime = 0;
        display.displayBitmap(BITMAP_STARTUP);
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

  if (rotary.isPaused() && status == Status::IDLE) {
    rotary.resume();
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
      setGuess(guess, false);
      return true;
    }
  }
  return false;
}

bool DiceGadget::checkStartButton() {
  bool pressed = activityControls.buttonPressed(START_BUTTON);
  if (pressed) {
    LOG("start button pressed.");
  }
  return pressed;
}

void DiceGadget::setGuess(int guess, bool updateDisplay) {
  if (currentGuess != guess) {
    LOG("set guess: %d.", guess);
    currentGuess = guess;
    display.putParam(PARAMETER_GUESS, currentGuess);
    if (updateDisplay) {
      display.displayMessage(MESSAGE_GUESS);
    }
  }
}

void DiceGadget::setJingle(int jingle) {
  if (selectedJingle != jingle) {
    selectedJingle = jingle;
    LOG("set jingle sound: %d.", selectedJingle);
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
  currentNumber = 0;
  currentLanguage = 0;
  startTime = 0;
  startTimeIndicator = 0;
  sound.setStatusHandler(nullptr);
  resetDisplayTime = millis() + RESET_DISPLAY_TIMEOUT;
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
  LOG("rotary event: status=%d, left=(%d,%ld), right=(%d,%ld)", status, (int)rotaryStatus[0].pressed, rotaryStatus[0].value, (int)rotaryStatus[1].pressed, rotaryStatus[1].value);
  rotary.pause();

  if (!rotaryStatus[ROTARY_ENCODER_RIGHT].pressed) {
    uint32_t jingle = rotaryStatus[ROTARY_ENCODER_LEFT].value / 2;
    setJingle(INITIAL_JINGLE + (abs(jingle) % JINGLES_COUNT));
  }

  if (status == Status::IDLE) {
    if (rotaryStatus[ROTARY_ENCODER_RIGHT].pressed) {
      status = Status::START_REQUESTED;
    }
    else {
      if (currentGuess || rotaryStatus[ROTARY_ENCODER_RIGHT].value) {
        int num = (int)((rotaryStatus[ROTARY_ENCODER_RIGHT].value / 2) % 6);
        int guess = num >= 0 ? num + 1 : 7 + num;
        setGuess(guess, true);
      }
    }
  }

  return false;
}
