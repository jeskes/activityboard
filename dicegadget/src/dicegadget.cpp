#include "dicegadget.h"

const uint8_t indicators[] = {INDICATOR_LED_1, INDICATOR_LED_2, INDICATOR_LED_3, INDICATOR_LED_4, INDICATOR_LED_5, INDICATOR_LED_6};
const uint8_t countIndicators = sizeof(indicators) / sizeof(uint8_t);

DiceGadget::DiceGadget()
    : core(DICEGADGET_ID),
      input(core),
      status(Status::IDLE),
      currentGuess(0),
      currentNumber(0),
      currentJingle(INITIAL_JINGLE),
      startedTime(0),
      indicatingTime(0),
      displayResetPending(0) {
}

void DiceGadget::setup() {
  GadgetBase::setup();

  for (int idx = 0; idx < countIndicators; idx++) {
    pinMode(indicators[idx], OUTPUT);
  }

  randomSeed(analogRead(NOISE_PIN));

  core.setup(CORE_STATUS_IRQ);
  input.setup();

  core.init(STARTUP_BITMAP);
  core.playSound(STARTUP_TRACK);
  activityControls.blink(indicators, countIndicators, 2, 200);
  core.setStatusHandler(this);

  LOG("Dice Gadget started.");
}

void DiceGadget::loop() {
  GadgetBase::loop();
  core.loop();
  stateMachine();
  showIndicators();
}

void DiceGadget::stateMachine() {
  Input in = input.get();
  if (in == StdInput::STOP) {
    reset();
    core.displayBitmap(STARTUP_BITMAP);
  }

  switch (status) {
    case Status::IDLE:
      checkDisplayReset();
      handleIdleInput(in);
      break;
    case Status::START_REQUESTED:
      throwDice();
      break;
    case Status::JINGLE_SOUND_PLAYING:
      activityControls.blink(indicators, countIndicators, 1, BLINK_RATE);
      break;
    case Status::JINGLE_SOUND_ENDED:
      status = Status::NUMBER_SOUND_PLAYING;
      core.displayMessage(NUMBER_MESSAGE);
      playNumber(currentNumber);
      break;
    case Status::NUMBER_SOUND_ENDED:
      status = Status::NUMBER_INDICATING;
      indicatingTime = millis();
      if (currentGuess) {
        core.displayMessage(currentGuess == currentNumber ? MATCH_MESSAGE : SORRY_MESSAGE);
        core.playSound(200 + (currentGuess == currentNumber ? 1 : 2));
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

void DiceGadget::handleIdleInput(Input in) {
  if (1 <= in && in <= 9) {
    setGuess(in);
    return;
  }
  if (in == StdInput::START) {
    LOG("start button pressed.");
    status = Status::START_REQUESTED;
    return;
  }
  if (in == DiceInput::JINGLE) {
    setJingle(currentJingle);
    return;
  }
  if (in == DiceInput::NEXT_JINGLE) {
    setJingle((currentJingle + 1) % JINGLES_COUNT);
    return;
  }
  if (in == DiceInput::NEXT_GUESS) {
    LOG("guess button pressed");
    int guess = currentGuess + 1;
    setGuess(guess > 6 ? 1 : guess);
    return;
  }
}

void DiceGadget::setGuess(int guess) {
  if (currentGuess != guess) {
    LOG("set guess: %d", guess);
    currentGuess = guess;
    playNumber(guess);
    guessIndicatingTime = millis() + INDICATING_TIMEOUT;
    core.putParam(GUESS_PARAMETER, currentGuess);
    core.displayMessage(GUESS_MESSAGE);
    scheduleDisplayReset();
  }
}

void DiceGadget::setJingle(int jingle) {
  if (currentJingle != jingle) {
    currentJingle = jingle;
    LOG("set jingle: %d", currentJingle);
    playJingle();
    core.putParam(JINGLE_PARAMETER, currentJingle);
    core.displayMessage(JINGLE_MESSAGE);
    scheduleDisplayReset();
  }
}

void DiceGadget::throwDice() {
  cancelDisplayReset();
  startedTime = millis();
  currentNumber = random(1, countIndicators + 1);
  playJingle();
  core.putParam(NUMBER_PARAMETER, currentNumber);
  core.displayMessage(STARTED_MESSAGE);
  status = Status::JINGLE_SOUND_PLAYING;
  guessIndicatingTime = 0;
}

void DiceGadget::reset() {
  currentNumber = 0;
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

  if (currentGuess && guessIndicatingTime && guessIndicatingTime > millis()) {
    digitalWrite(indicators[currentGuess - 1], HIGH);
  }
}

void DiceGadget::playNumber(int number) {
  int language = random(0, LANGUAGES_COUNT);
  core.playSound(10 * language + number);
}

void DiceGadget::playJingle() {
  core.playSound(JINGLE_START + currentJingle);
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

void DiceGadget::handleCoreStatus(CoreStatus coreStatus) {
  PlayerState playerState = coreStatus.playerState;
  LOG("player state: %d", playerState);
  switch (status) {
    case Status::JINGLE_SOUND_PLAYING:
      if (playerState == PlayerState::IDLE) {
        LOG("jingle sound ended");
        status = Status::JINGLE_SOUND_ENDED;
      }
      break;
    case Status::NUMBER_SOUND_PLAYING:
      if (playerState == PlayerState::IDLE) {
        LOG("number sound ended");
        status = Status::NUMBER_SOUND_ENDED;
      }
      break;
    default:
      break;
  }
}
