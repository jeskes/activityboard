#pragma once

#include <button/button.h>
#include <core/core.h>
#include <modules/modules.h>
#include <numpad/numpad.h>

#include "const.h"

class DiceGadget : GadgetBase, CoreStatusHandler {
 public:
  DiceGadget();

  void setup();
  void loop();
  bool handleCoreStatus(CoreStatus status) override;

 private:
  ActivityCoreClient core;
  ActivityNumpadClient numpad;

  ActivityButton startButton;
  ActivityButton guessButton;
  ActivityButton jingleButton;

  void stateMachine();
  void showIndicators();
  void throwDice();
  void reset();

  void setGuess(int guess, bool updateDisplay);
  void setJingle(int jingle, bool updateDisplay);

  bool checkNumpad();
  bool checkStartButton();
  bool checkGuessButton();
  bool checkJingleButton();

  enum class Status : int {
    IDLE = 0,
    START_REQUESTED = 1,
    JINGLE_SOUND_PLAYING = 2,
    JINGLE_SOUND_ENDED = 3,
    NUMBER_SOUND_PLAYING = 4,
    NUMBER_SOUND_ENDED = 5,
    NUMBER_INDICATING = 6
  };

  Status status;

  int currentGuess;
  int currentNumber;
  int currentLanguage;
  int currentJingle;

  unsigned long startedTime;
  unsigned long indicatingTime;
  unsigned long displayResetPending;

  void checkDisplayReset();
  void scheduleDisplayReset();
  void cancelDisplayReset();
};
