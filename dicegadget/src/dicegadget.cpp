#include "dicegadget.h"

const uint8_t indicators[] = {INDICATOR_LED_1, INDICATOR_LED_2, INDICATOR_LED_3, INDICATOR_LED_4, INDICATOR_LED_5, INDICATOR_LED_6};
const uint8_t countIndicators = sizeof(indicators) / sizeof(uint8_t);

DiceGadget::DiceGadget()
    : core(DICEGADGET_ID, scheduler),
      startButton(),
      guessButton(),
      jingleButton(),
      status(Status::IDLE),
      currentGuess(0),
      currentNumber(0),
      currentJingle(INITIAL_JINGLE),
      startedTime(0),
      indicatingTime(0),
      displayResetPending(0) {
}

void DiceGadget::setup() {
  startButton.setup(START_BUTTON);
  guessButton.setup(GUESS_BUTTON);
  jingleButton.setup(JINGLE_BUTTON);

  for (int idx = 0; idx < countIndicators; idx++) {
    pinMode(indicators[idx], OUTPUT);
  }

  activityControls.blink(indicators, countIndicators, 2, 200);
  randomSeed(analogRead(NOISE_PIN));

  GadgetBase::setup();

  core.setup();
  numpad.setup();

  core.init(BITMAP_STARTUP);
  core.setStatusHandler(this);

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
      checkDisplayReset();
      if (checkNumpad() || checkStartButton()) {
        status = Status::START_REQUESTED;
      }
      else {
        checkGuessButton();
        checkJingleButton();
      }
      break;
    case Status::START_REQUESTED:
      throwDice();
      break;
    case Status::JINGLE_SOUND_PLAYING:
      activityControls.blink(indicators, countIndicators, 1, BLINK_RATE);
      break;
    case Status::JINGLE_SOUND_ENDED:
      status = Status::NUMBER_SOUND_PLAYING;
      core.playSound(10 * currentLanguage + currentNumber);
      core.displayMessage(MESSAGE_NUMBER);
      break;
    case Status::NUMBER_SOUND_ENDED:
      status = Status::NUMBER_INDICATING;
      indicatingTime = millis();
      if (currentGuess) {
        core.playSound(200 + (currentGuess == currentNumber ? 1 : 2));
        core.displayMessage(currentGuess == currentNumber ? MESSAGE_MATCH : MESSAGE_SORRY);
      }
      break;
    case Status::NUMBER_INDICATING:
      if (millis() > indicatingTime + INDICATING_TIMEOUT) {
        reset();
      }
    default:
      break;
  }

  if ((status == Status::JINGLE_SOUND_PLAYING) && (millis() > startedTime + JINGLE_PLAYING_TIMEOUT)) {
    LOG("jingle-playing-reset");
    status = Status::JINGLE_SOUND_ENDED;
  }
  if ((status == Status::NUMBER_SOUND_PLAYING) && (millis() > startedTime + NUMBER_PLAYING_TIMEOUT)) {
    LOG("number-playing-reset");
    status = Status::NUMBER_SOUND_ENDED;
  }
  if ((status > Status::START_REQUESTED) && (millis() > startedTime + AUTORESET_TIMEOUT)) {
    LOG("auto-reset");
    reset();
  }
}

bool DiceGadget::checkNumpad() {
  char ch = numpad.getChar();
  if (ch) {
    LOG("numpad key: %c", ch);
    int guess = ch - (int)'0';
    if (guess >= 0 && guess <= 6) {
      setGuess(guess, false);
      return true;
    }
    else if (ch == '*') {
      setJingle((currentJingle + 1) % JINGLES_COUNT, true);
    }
    else if (ch == '#') {
      return true;
    }
  }
  return false;
}

bool DiceGadget::checkStartButton() {
  bool pressed = startButton.pressed();
  if (pressed) {
    LOG("start button pressed.");
  }
  return pressed;
}

bool DiceGadget::checkGuessButton() {
  bool pressed = guessButton.pressed();
  if (pressed) {
    LOG("guess button pressed");
    setGuess((currentGuess + 1) % 7, true);
  }
  return pressed;
}

bool DiceGadget::checkJingleButton() {
  bool pressed = jingleButton.pressed();
  if (pressed) {
    LOG("jingle button pressed");
    setJingle((currentJingle + 1) % JINGLES_COUNT, true);
  }
  return pressed;
}

void DiceGadget::setGuess(int guess, bool updateDisplay) {
  if (currentGuess != guess) {
    LOG("set guess: %d", guess);
    currentGuess = guess;
    core.putParam(PARAMETER_GUESS, currentGuess);
    if (updateDisplay) {
      core.displayMessage(MESSAGE_GUESS);
      scheduleDisplayReset();
    }
  }
}

void DiceGadget::setJingle(int jingle, bool updateDisplay) {
  if (currentJingle != jingle) {
    currentJingle = jingle;
    LOG("set jingle: %d", currentJingle);
    core.putParam(PARAMETER_JINGLE, currentJingle);
    core.playSound(JINGLE_START + currentJingle);
    if (updateDisplay) {
      core.displayMessage(MESSAGE_JINGLE);
      scheduleDisplayReset();
    }
  }
}

void DiceGadget::throwDice() {
  cancelDisplayReset();
  startedTime = millis();
  currentNumber = random(1, countIndicators + 1);
  currentLanguage = random(0, LANGUAGES_COUNT);
  core.putParam(PARAMETER_NUMBER, currentNumber);
  core.displayMessage(MESSAGE_STARTED);
  core.playSound(JINGLE_START + currentJingle);
  status = Status::JINGLE_SOUND_PLAYING;
}

void DiceGadget::reset() {
  currentNumber = 0;
  currentLanguage = 0;
  startedTime = 0;
  indicatingTime = 0;
  scheduleDisplayReset();
  status = Status::IDLE;
}

void DiceGadget::showIndicators() {
  for (int idx = 0; idx < countIndicators; idx++) {
    int level = (idx == currentNumber - 1) ? HIGH : LOW;
    digitalWrite(indicators[idx], level);
  }
}

bool DiceGadget::handleCoreStatus(CoreStatus coreStatus) {
  PlayerState playerState = coreStatus.playerState;
  // LOG("dice-status=%d, player-state=%d", status, playerState);
  if (/*playerMonitor == PlayerMonitor::PENDING && */ playerState == PlayerState::IDLE) {
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

void DiceGadget::checkDisplayReset() {
  if (displayResetPending && (millis() > displayResetPending)) {
    displayResetPending = 0;
    core.displayBitmap(BITMAP_STARTUP);
  }
}

void DiceGadget::scheduleDisplayReset() {
  displayResetPending = millis() + RESET_DISPLAY_TIMEOUT;
}

void DiceGadget::cancelDisplayReset() {
  displayResetPending = 0;
}
