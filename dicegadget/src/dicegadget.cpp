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

  randomSeed(analogRead(NOISE_PIN));

  GadgetBase::setup();

  core.setup();
  numpad.setup();

  core.init(STARTUP_BITMAP);
  // core.playSound(STARTUP_TRACK);
  activityControls.blink(indicators, countIndicators, 2, 200);
  core.setStatusHandler(this);
  core.pauseStatus();

  LOG("Dice Gadget started.");
}

void DiceGadget::loop() {
  GadgetBase::loop();
  stateMachine();
  showIndicators();
}

void DiceGadget::stateMachine() {
  if (numpad.getChar() == 'D') {
    reset();
    core.displayBitmap(STARTUP_BITMAP);
  }

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
      playNumber(currentNumber);
      core.displayMessage(NUMBER_MESSAGE);
      break;
    case Status::NUMBER_SOUND_ENDED:
      status = Status::NUMBER_INDICATING;
      indicatingTime = millis();
      if (currentGuess) {
        core.playSound(currentGuess == currentNumber ? SUCCESS_TRACK : FAILURE_TRACK);
        core.displayMessage(currentGuess == currentNumber ? MATCH_MESSAGE : SORRY_MESSAGE);
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
    if (guess > 0 && guess <= 6) {
      setGuess(guess);
      return false;
    }
    else if (ch == 'A' || ch == '#') {
      return true;
    }
    else if (ch == 'B') {
      setJingle(currentJingle);
    }
    else if (ch == 'C' || ch == 'C') {
      setJingle((currentJingle + 1) % JINGLES_COUNT);
    }
    else if (ch == 'D') {
      reset();
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
    int guess = currentGuess + 1;
    setGuess(guess > 6 ? 1 : guess);
  }
  return pressed;
}

bool DiceGadget::checkJingleButton() {
  bool pressed = jingleButton.pressed();
  if (pressed) {
    LOG("jingle button pressed");
    setJingle((currentJingle + 1) % JINGLES_COUNT);
  }
  return pressed;
}

void DiceGadget::setGuess(int guess) {
  if (currentGuess != guess) {
    LOG("set guess: %d", guess);
    currentGuess = guess;
    // playNumber(guess);
    guessIndicatingTime = millis() + INDICATING_TIMEOUT;
    core.putParameter(GUESS_PARAMETER, currentGuess);
    core.displayMessage(GUESS_MESSAGE);
    scheduleDisplayReset();
  }
}

void DiceGadget::setJingle(int jingle) {
  if (currentJingle != jingle) {
    currentJingle = jingle;
    LOG("set jingle: %d", currentJingle);
    playJingle();
    core.putParameter(JINGLE_PARAMETER, currentJingle);
    core.displayMessage(JINGLE_MESSAGE);
    scheduleDisplayReset();
  }
}

void DiceGadget::throwDice() {
  cancelDisplayReset();
  startedTime = millis();
  currentNumber = random(1, countIndicators + 1);
  playJingle();
  core.putParameter(NUMBER_PARAMETER, currentNumber);
  core.displayMessage(STARTED_MESSAGE);
  status = Status::JINGLE_SOUND_PLAYING;
  guessIndicatingTime = 0;
}

void DiceGadget::reset() {
  currentNumber = 0;
  startedTime = 0;
  indicatingTime = 0;
  core.stopSound();
  scheduleDisplayReset();
  status = Status::IDLE;
}

void DiceGadget::showIndicators() {
  for (int idx = 0; idx < countIndicators; idx++) {
    int level = (idx == currentNumber - 1) ? HIGH : LOW;
    digitalWrite(indicators[idx], level);
  }

  if (currentGuess && guessIndicatingTime && guessIndicatingTime > millis()) {
    digitalWrite(indicators[currentGuess - 1], HIGH);
  }
}

void DiceGadget::playNumber(int number) {
  int language = random(0, LANGUAGES_COUNT);
  core.playSound(makeNumberTrackId(language, number));
}

void DiceGadget::playJingle() {
  core.playSound(makeJingleTrackId(currentJingle));
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
    core.displayBitmap(STARTUP_BITMAP);
  }
}

void DiceGadget::scheduleDisplayReset() {
  displayResetPending = millis() + RESET_DISPLAY_TIMEOUT;
}

void DiceGadget::cancelDisplayReset() {
  displayResetPending = 0;
}

TrackId DiceGadget::makeJingleTrackId(int jingle) {
  static char buffer[10];
  sprintf(buffer,"%03d", JINGLE_START + jingle);
  return buffer;
}

TrackId DiceGadget::makeNumberTrackId(int language, int number) {
  static char buffer[10];
  sprintf(buffer,"%03d", 10 * language + number);
  return buffer;
}